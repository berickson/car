
let viewer = (function () {
    "use strict";
    let scene = new THREE.Scene(),
        renderer = new THREE.WebGLRenderer({ antialias: true }),
        ambient_light = new THREE.AmbientLight(0x555555),
        point_light,
        sun = new THREE.DirectionalLight(0xffffff, 0.3),
        ground,
        camera,
        car = new THREE.Object3D(),
        scan_mesh = new THREE.Object3D(),
        interaction,
        scan = null,
        stats = new Stats(),
        controls,
        last_scan_number = -1,
        loader = new THREE.TextureLoader(),
        lidar_elements,
        lidar_x_pos = 0.57 / 2 - 0.0635,
        label_renderer = new THREE.CSS2DRenderer();

    label_renderer.setSize( window.innerWidth, window.innerHeight );
    label_renderer.domElement.style.position = 'absolute';
    label_renderer.domElement.style.top = 0;
    
    

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap;

    // ground
    {
        let ground_texture = loader.load("TexturesCom_WoodPlanksBare0498_5_seamless_S.png");
        ground_texture.repeat.set(5000, 5000);
        ground_texture.wrapS = THREE.RepeatWrapping;
        ground_texture.wrapT = THREE.RepeatWrapping;
        let ground_material = new THREE.MeshLambertMaterial({ map: ground_texture });
        ground = new THREE.Mesh(
            new THREE.PlaneGeometry(20000, 20000, 200, 200),
            ground_material); //new THREE.MeshLambertMaterial({color:0xF5F5DC, side:THREE.DoubleSide}));
        ground.receiveShadow = true;
        ground.castShadow = false;
    }
    scene.add(ground);


    // color, intensity, distance, decay;
    point_light = new THREE.PointLight(0xffffff, 5, 50, 2);
    point_light.castShadow = true;
    scene.add(point_light);
    point_light.position.set(5, -5, 30);
    point_light.shadow.mapSize.width = 512;  // default
    point_light.shadow.mapSize.height = 512; // default
    point_light.shadow.camera.near = 0.;
    point_light.shadow.camera.far = 500; // default


    {
        let materialArray = [];
        //let images = ['px.jpg', 'nx.jpg', 'py.jpg', 'ny.jpg', 'pz.jpg', 'nz.jpg'];
        //let images = ['dawnmountain-xpos.png', 'dawnmountain-xneg.png', 'dawnmountain-ypos.png', 'dawnmountain-yneg.png','dawnmountain-zpos.png', 'dawnmountain-zneg.png'];
        let images = ['Daylight Box_Right.bmp', 'Daylight Box_Left.bmp', 'Daylight Box_Top.bmp', 'Daylight Box_Bottom.bmp', 'Daylight Box_Front.bmp', 'Daylight Box_Back.bmp'];
        for (let i = 0; i < 6; ++i) {
            materialArray.push(new THREE.MeshBasicMaterial({ map: loader.load(images[i]) }));
            materialArray[i].side = THREE.BackSide;
        }

        let skybox = new THREE.Mesh(
            new THREE.CubeGeometry(5000, 5000, 5000, 1, 1, 1),
            materialArray);

        skybox.geometry.rotateX(Math.PI / 2);
        skybox.castShadow = false;
        scene.add(skybox);
    }

    //Set up shadow properties for the light
    sun.shadow.mapSize.width = 512;  // default
    sun.shadow.mapSize.height = 512; // default
    sun.shadow.camera.near = 0.;
    sun.shadow.camera.far = 500; // default

    sun.position.z = 1000; // shine down from z
    sun.castShadow = true;

    function set_car_state(car_state) {
        car.position.x = car_state.front_x - Math.cos(car_state.heading) * 0.3;
        car.position.y = car_state.front_y - Math.sin(car_state.heading) * 0.3;
        car.rotation.z = car_state.heading;
        if(controls) {
            controls.update();
            //controls.target.set(car.position);
        }
    }


    function set_run_path(path) {
        try {
            scene.remove(scene.getObjectByName('run_path'))
        } catch { }
        let path_mesh = new THREE.Object3D();
        path_mesh.name = 'path';
        let waypoint_template = new THREE.Mesh(
            new THREE.SphereGeometry(0.015, 10, 10),
            new THREE.MeshLambertMaterial({ color: 0x0000FF, transparent: false, opacity: 1 })
        );
        waypoint_template.castShadow = true;
        waypoint_template.receiveShadow = false;

        // delete or they show in background
        while (label_renderer.domElement.firstChild) {
            label_renderer.domElement.removeChild(label_renderer.domElement.firstChild);
        }

        for (let i = 0; i < path.length; ++i) {
            let node = path[i];
            let waypoint = waypoint_template.clone();
            waypoint.position.x = node.x;
            waypoint.position.y = node.y;
            waypoint.name = "waypoint" + i;
            waypoint.node = node;
            waypoint.cursor = 'pointer';
//             waypoint.on('click', function(ev) {
//                 car_vm.node_clicked(ev, ev.currentTarget.node);
//             });            path_mesh.add(waypoint);
//             let t = car_vm.has_road_sign(node);
//             //if(node.road_sign_command && node.road_sign_command.length > 0 || node.road_sign_label && node.road_sign_label.length > 0) {
//             if(t) {
//                 let road_sign = new THREE.Mesh(
//                     new THREE.SphereGeometry(0.03,10,10),
//                     new THREE.MeshLambertMaterial({ color: 0xff0000})
//                 );
//                 let post = new THREE.Mesh(
//                     new THREE.BoxGeometry(0.015,0.015, 0.4),
//                     new THREE.MeshStandardMaterial({ color: 0x888888})
//                 );
//                 post.position.z = 0.2
//                 //let label = makeTextSprite(node.road_sign_label + ": " + node.road_sign_command);

//                 let earthDiv = document.createElement( 'div' );
// 				earthDiv.textContent = node.road_sign_label + ": " + node.road_sign_command;
//                 earthDiv.style.marginTop = '-1em';
//                 earthDiv.className="label2d";
//                 document.body.appendChild(earthDiv);
//                 let earthLabel = new THREE.CSS2DObject( earthDiv );
// 				earthLabel.position.set( 0, 0, 0.42 );
//                 //earth.add( earthLabel );
//                 //earthLabel.setSize(100,100);
//                 waypoint.add(earthLabel)
                

//                 //road_sign.add(label);
//                 road_sign.position.z = 0.4;
//                 waypoint.add(post);
//                 waypoint.add(road_sign);
//             }
        }
        path_mesh.position.z = 0.1;

        scene.add(path_mesh);
    }


    function set_path(path) {
        try {
            scene.remove(scene.getObjectByName('path'))
        } catch { }

        let path_mesh = new THREE.Object3D();
        path_mesh.name = 'path';
        let waypoint_template = new THREE.Mesh(
            new THREE.SphereGeometry(0.015, 10, 10),
            new THREE.MeshLambertMaterial({ color: 0x0000FF, transparent: false, opacity: 1 })
        );
        waypoint_template.castShadow = true;
        waypoint_template.receiveShadow = false;

        // delete or they show in background
        while (label_renderer.domElement.firstChild) {
            label_renderer.domElement.removeChild(label_renderer.domElement.firstChild);
        }

        for (let i = 0; i < path.length; ++i) {
            let node = path[i];
            let waypoint = waypoint_template.clone();
            waypoint.position.x = node.x;
            waypoint.position.y = node.y;
            waypoint.name = "waypoint" + i;
            waypoint.node = node;
            waypoint.cursor = 'pointer';
            waypoint.on('click', function(ev) {
                car_vm.node_clicked(ev, ev.currentTarget.node);
            });            path_mesh.add(waypoint);
            let t = car_vm.has_road_sign(node);
            //if(node.road_sign_command && node.road_sign_command.length > 0 || node.road_sign_label && node.road_sign_label.length > 0) {
            if(t) {
                let road_sign = new THREE.Mesh(
                    new THREE.SphereGeometry(0.03,10,10),
                    new THREE.MeshLambertMaterial({ color: 0xff0000})
                );
                let post = new THREE.Mesh(
                    new THREE.BoxGeometry(0.015,0.015, 0.4),
                    new THREE.MeshStandardMaterial({ color: 0x888888})
                );
                post.position.z = 0.2
                //let label = makeTextSprite(node.road_sign_label + ": " + node.road_sign_command);

                let earthDiv = document.createElement( 'div' );
				earthDiv.textContent = node.road_sign_label + ": " + node.road_sign_command;
                earthDiv.style.marginTop = '-1em';
                earthDiv.className="label2d";
                document.body.appendChild(earthDiv);
                let earthLabel = new THREE.CSS2DObject( earthDiv );
				earthLabel.position.set( 0, 0, 0.42 );
                //earth.add( earthLabel );
                //earthLabel.setSize(100,100);
                waypoint.add(earthLabel)
                

                //road_sign.add(label);
                road_sign.position.z = 0.4;
                waypoint.add(post);
                waypoint.add(road_sign);
            }
        }
        path_mesh.position.z = 0.1;

        scene.add(path_mesh);
    }

    function initScene() {
        label_renderer.domElement.className = "noclick";

        let element = document.getElementById("webgl-container");
        renderer.setSize(element.offsetWidth, element.offsetHeight);
        element.appendChild(renderer.domElement);
        element.appendChild( label_renderer.domElement );
        scene.add(ambient_light);
        //scene.add(point_light)
        scene.add(sun);
        scene.add(scan_mesh);
        camera = new THREE.PerspectiveCamera(
            35, // FOV, degrees
            element.offsetWidth / element.offsetHeight, // aspect ratio
            .1,   // near cutoff
            1000000 // far cutoff
        );
        camera.position.z = 2;
        camera.position.y = -0;
        camera.position.x = -5;
        camera.up = new THREE.Vector3(0, 0, 1);

        controls = new THREE.OrbitControls(camera, renderer.domElement);
        controls.target.set(0, 0, 0); // view direction perpendicular to XY-plane
        controls.enableRotate = true;
        controls.enableZoom = true; // optional
        controls.maxPolarAngle = 89.5 * Math.PI / 180;  // keep above ground


        car.add(camera);
        interaction = new THREE.Interaction(renderer, scene, camera);
        
        {
            let l = 0.57, w = 0.305, h = 0.19; // todo: move to car related structure
            let body = new THREE.Mesh(
                new THREE.BoxGeometry(l, w, h),
                new THREE.MeshStandardMaterial({ color: 0xFF8C00, metalness:0.1, roughness: 0.9 })
            );
            car.add(body);
            {
                let lidar_h = 0.02;
                let lidar_w = 0.06;
                let lidar_unit = new THREE.Mesh(
                    new THREE.CylinderGeometry( lidar_w, lidar_w, lidar_h, 20, 3  ),
                    new THREE.MeshStandardMaterial({ color: 0x222222, roughness:0.5, metalness:0.2 }));
                lidar_unit.rotateX(Math.PI/2);
                lidar_unit.position.x = lidar_x_pos;
                lidar_unit.position.z = (h+lidar_h)/2;
                car.add(lidar_unit);
            }
            


            // arrow on car
            {
                let length = l - 0.01;
                let arrow = new THREE.Mesh(
                    new THREE.ConeBufferGeometry(w / 4, length, 20),
                    new THREE.MeshStandardMaterial({ color: 0x777777 }),
                );
                arrow.position.x = 0;
                arrow.position.z = h / 2;
                arrow.geometry.rotateZ(-Math.PI / 2);
                car.add(arrow);
            }
            car.position.z = h / 2 + 0.03;
            car.position.x = -l / 2;
            body.castShadow = true;
            body.receiveShadow = false;
        }
        scene.add(car);

        let lidar_template = new THREE.Mesh(
            new THREE.BoxGeometry(0.03, 0.03, 0.35),
            new THREE.MeshLambertMaterial({ color: 0x00ff00 }));
        lidar_template.position.z = 0.35/2;
        lidar_elements = [];
        for (let i = 0; i < 360; i++) {
            let lidar_element = lidar_template.clone();
            lidar_element.visible = false;
            lidar_elements.push(lidar_element);
            scan_mesh.add(lidar_element);
        }
        scene.add(scan_mesh);

        stats.showPanel(0); // 0: fps, 1: ms, 2: mb, 3+: custom
        document.body.appendChild(stats.dom);
        on_resize();
        render();
        get_scan();
    }

    function get_scan() {
        let xmlhttp = new XMLHttpRequest();
        let url = "car/get_scan?since="+last_scan_number;

        xmlhttp.onreadystatechange = function () {
            if (this.readyState == 4) { // done
                let timeout_ms = 1000;
                if (this.status == 200) { // ok
                    try {
                        scan = JSON.parse(this.responseText);
                        if (scan !== null) {
                            last_scan_number = scan.number;
                            for (let i = 0; i < scan.angle.length; i++) {
                                let l = scan.distance_meters[i];
                                let theta = scan.angle[i] + scan.pose_theta[i];
                                let x = scan.pose_x[i];
                                let y = scan.pose_y[i];
                                let lidar_element = lidar_elements[i];
                                if (l > 0) {
                                    lidar_element.visible = true;
                                    lidar_element.position.x = x + Math.cos(theta) * l;
                                    lidar_element.position.y = y + Math.sin(theta) * l;
                                }
                                else {
                                    lidar_element.visible = false;
                                }
                            }
                            scan_mesh.position.x = 0;//car.position.x + lidar_x_pos * Math.cos(car.rotation.z);
                            scan_mesh.position.y = 0;//car.position.y + lidar_x_pos * Math.sin(car.rotation.z);
                            scan_mesh.rotation.z = 0;//car.rotation.z;
                        }
                        timeout_ms = 0; // force fast update for next scan
                    } catch (e) {}
                } 
                window.setTimeout(get_scan, timeout_ms);
                
            }
        };
        xmlhttp.open("GET", url, true);
        xmlhttp.send();
    }

    function on_resize() {
        let element = document.getElementById("webgl-container");

        // set the container height small so it isn't considered while
        // calculating height of the rest of the document
        element.style.height= 3+"px";

        // get full document height / width
        let html = document.getElementById("html");
        let h = Math.max(window.innerHeight, html.scrollHeight);
        let w = Math.max(window.innerWidth, html.scrollWidth);

        // set element to that size, and update rendering objects
        element.style.height= h+"px";
        camera.aspect = w/h;
        camera.updateProjectionMatrix();
        renderer.setSize(w, h);
    }

    function render() {
        stats.begin();
        stats.end();
        renderer.render(scene, camera);
        label_renderer.render(scene, camera);
        requestAnimationFrame(render);  // force to call again at next repaint
    }

    window.addEventListener("resize", on_resize);
    window.onload = initScene;

    return {
        car: car,
        scene: scene,
        set_path: set_path,
        set_run_path: set_run_path,
        set_car_state: set_car_state
    }


})();

function get_car_scope() {
    var scope = angular.element(document.getElementById("html")).scope();
    return scope;
}