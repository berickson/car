let example = (function(){
    "use strict";

    let scene = new THREE.Scene(),
    renderer = new THREE.WebGLRenderer({antialias:true}),
    light = new THREE.AmbientLight(0x555555),
    point_light,
    sun = new THREE.DirectionalLight(0xffffff,0.3),
    ground,
    camera,
    path,
    car,
    sky,
    scan = null,
    stats = new Stats(),
    controls,
    loader = new THREE.TextureLoader(),
    lidar_elements;

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap;

    // ground
    {
        let ground_texture = loader.load("TexturesCom_WoodPlanksBare0498_5_seamless_S.png");
    ground_texture.repeat.set(5000, 5000);
    ground_texture.wrapS = THREE.RepeatWrapping;
    ground_texture.wrapT = THREE.RepeatWrapping;
        let ground_material = new THREE.MeshLambertMaterial({map: ground_texture});
        ground = new THREE.Mesh(
            new THREE.PlaneGeometry(20000,20000,200,200),
            ground_material); //new THREE.MeshLambertMaterial({color:0xF5F5DC, side:THREE.DoubleSide}));
        ground.receiveShadow = true;
        ground.castShadow = false;
    }
    scene.add(ground);


    // color, intensity, distance, decay;
    point_light = new THREE.PointLight( 0xffffff, 5, 50, 2 );
    point_light.position.set( 5, -5, 30 );
    scene.add( point_light );

    {
        let materialArray = [];
        //let images = ['px.jpg', 'nx.jpg', 'py.jpg', 'ny.jpg', 'pz.jpg', 'nz.jpg'];
        //let images = ['dawnmountain-xpos.png', 'dawnmountain-xneg.png', 'dawnmountain-ypos.png', 'dawnmountain-yneg.png','dawnmountain-zpos.png', 'dawnmountain-zneg.png'];
        let images = ['Daylight Box_Right.bmp', 'Daylight Box_Left.bmp', 'Daylight Box_Top.bmp', 'Daylight Box_Bottom.bmp', 'Daylight Box_Front.bmp','Daylight Box_Back.bmp'];
        for(let i=0; i < 6; ++i) {
            materialArray.push(new THREE.MeshBasicMaterial( { map: loader.load( images[i] ) }));            
            materialArray[i].side = THREE.BackSide;
         }

        let skybox = new THREE.Mesh( 
            new THREE.CubeGeometry( 5000, 5000, 5000, 1, 1, 1 ), 
            materialArray);

        skybox.geometry.rotateX(Math.PI/2);
        scene.add( skybox );
    }

    //Set up shadow properties for the light
    sun.shadow.mapSize.width = 512;  // default
    sun.shadow.mapSize.height = 512; // default
    sun.shadow.camera.near = 0.5;    // default
    sun.shadow.camera.far = 500; // default

    sun.position.z = 1000; // shine down from z
    sun.castShadow = true;

    function initScene(){
        renderer.setSize(window.innerWidth, window.innerHeight);
        document.getElementById("webgl-container").appendChild(renderer.domElement);
        scene.add(light);
        scene.add(point_light)
        scene.add(sun);
        camera = new THREE.PerspectiveCamera(
            35, // FOV, degrees
            window.innerWidth / window.innerHeight, // aspect ratio
            .1,   // near cutoff
            1000000 // far cutoff
        );
        camera.position.z = 2;
        camera.position.y = -0;
        camera.position.x = -5;
        camera.up = new THREE.Vector3(0,0,1);

        controls = new THREE.OrbitControls( camera, renderer.domElement );
        controls.target.set( 0, 0, 0 ); // view direction perpendicular to XY-plane
        controls.enableRotate = true;
        controls.enableZoom = true; // optional
        controls.maxPolarAngle = 89.5 * Math.PI/180;  // keep above ground


        scene.add(camera);

        car = new THREE.Object3D();
        {
            let l=0.65, w=0.3, h=0.15;
            let body = new THREE.Mesh(
                new THREE.BoxGeometry(l, w, h),
                new THREE.MeshLambertMaterial({color:0xFF8C00})
            );
            car.add(body);
            let edges = new THREE.LineSegments( 
                new THREE.EdgesGeometry(body.geometry), 
                new THREE.LineBasicMaterial( { color: 0xffffff } ));
            //car.add(edges);
            
            
            // arrow on car
            {
                let length = l-0.01;
                let arrow = new THREE.Mesh(
                  new THREE.ConeBufferGeometry(  w/4, length, 20 ),
                  new THREE.MeshLambertMaterial({color: 0x777777}),
                );
                arrow.position.x = 0;
                arrow.position.z = h/2;
                arrow.geometry.rotateZ(-Math.PI/2);
                car.add( arrow );
            }
            car.position.z = h/2 + 0.03;
            car.position.x = -l/2;
            body.castShadow = true;
            body.receiveShadow = false;
        }
        scene.add(car);



        path = new THREE.Object3D();
        for(let i = 0; i < test_path.length; ++i) {
            let node = test_path[i];
            let waypoint = new THREE.Mesh(
                new THREE.SphereGeometry(0.015,10,10),
                new THREE.MeshLambertMaterial({color: 0x0000FF, transparent:false, opacity:1})
            );
            waypoint.position.x = node.x;
            waypoint.position.y = node.y;
            waypoint.name = "waypoint"+i;
            waypoint.castShadow = true;
            waypoint.receiveShadow = false;
            path.add(waypoint);
        }
        path.position.z = 0.1;

        scene.add(path);

        let lidar_template = new THREE.Mesh(
            new THREE.BoxGeometry(0.03,0.03,0.5),
            new THREE.MeshLambertMaterial({color: 0x00ff00}));
        lidar_elements = [];
        for(let i = 0; i < 360; i++) {
            let lidar_element = lidar_template.clone();
            let l = Math.random() * 50 + 1;
            lidar_element.position.x = l * Math.cos(i*Math.PI/180.);
            lidar_element.position.y = l * Math.sin(i*Math.PI/180.);
            lidar_element.position.z = 0.25;
            lidar_element.l = l;
            lidar_element.visible = false;
            lidar_elements.push(lidar_element)
            scene.add(lidar_element);
        }

        stats.showPanel( 0 ); // 0: fps, 1: ms, 2: mb, 3+: custom
        document.body.appendChild( stats.dom );

        render();
        get_scan();
    }

    function get_scan() {
        let xmlhttp = new XMLHttpRequest();
        let url = "car/get_scan";

        xmlhttp.onreadystatechange = function() {
            if (this.readyState == 4)  { // done
                if ( this.status == 200) { // ok
                    try {
                        scan = JSON.parse(this.responseText);
                        if(scan !== null ) {
                            for(let i = 0; i < scan.angle.length; i++)  {
                                let l = scan.distance_meters[i];
                                let theta = scan.angle[i];
                                let lidar_element = lidar_elements[i];
                                if(l>0) {
                                    lidar_element.visible = true;
                                    lidar_element.position.x = Math.cos(theta) * l;
                                    lidar_element.position.y = Math.sin(theta) * l;
                                }
                                else {
                                    lidar_element.visible = false;
                                }
                            }
                        }
                    } catch (e) {}
                }
                window.setTimeout(get_scan, 0);
            }
        };
        xmlhttp.open("GET", url, true);
        xmlhttp.send();
    }

    function render() {
        stats.begin();
        stats.end();
        renderer.render(scene, camera);
        requestAnimationFrame(render);  // force to call again at next repaint
    }

    window.onload = initScene;

    return {
        scene : scene
    }


})();
