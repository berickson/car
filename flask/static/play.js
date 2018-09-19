var example = (function(){
    "use strict";

    var scene = new THREE.Scene(),
    renderer = new THREE.WebGLRenderer({antialias:true}),
    light = new THREE.AmbientLight(0x555555),
    point_light,
    sun = new THREE.DirectionalLight(0xffffff,0.3),
    ground,
    ground_texture,
    ground_material,
    camera,
    path,
    car,
    controls;
    var loader = new THREE.TextureLoader();
    ground_texture = loader.load("TexturesCom_WoodPlanksBare0498_5_seamless_S.png");
    ground_texture.repeat.set(5000, 5000);
    //ground_texture = THREE.ImageUtils.loadTexture("TexturesCom_ConcreteFloors0054_1_seamless_S.jpg");
    //ground_texture.repeat.set(4000, 4000);
    //ground_texture = THREE.ImageUtils.loadTexture("TexturesCom_AsphaltCloseups0081_1_seamless_S.jpg");
    //ground_texture.repeat.set(200, 200);
    ground_texture.wrapS = THREE.RepeatWrapping;
    ground_texture.wrapT = THREE.RepeatWrapping;
    ground_material = new THREE.MeshLambertMaterial({map: ground_texture});

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap; // default THREE.PCFShadowMa

    // color, intensity, distance, decay;
    point_light = new THREE.PointLight( 0xffffff, 5, 50, 2 );
    point_light.position.set( 5, -5, 30 );
    scene.add( point_light );

    var sky_texture = loader.load("TexturesCom_Skies0306_M.jpg");
    var sky_geometry =  new THREE.SphereGeometry(100000,25,25);
    var sky = new THREE.Mesh(sky_geometry,
              new THREE.MeshBasicMaterial({map:sky_texture}));
    sky.material.side = THREE.BackSide;
    scene.add(sky);

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
        controls.maxPolarAngle = 85 * Math.PI/180;  // keep above ground


        scene.add(camera);

        car = new THREE.Object3D();
        {
            var l=0.65, w=0.3, h=0.15;
            var body = new THREE.Mesh(
                new THREE.BoxGeometry(l, w, h),
                new THREE.MeshLambertMaterial({color:0xFF8C00})
            );
            car.add(body);
            var edges = new THREE.LineSegments( 
                new THREE.EdgesGeometry(body.geometry), 
                new THREE.LineBasicMaterial( { color: 0xffffff } ));
            car.add(edges);
            car.position.z = h/2 + 0.03;
            car.position.x = -l/2;
            body.castShadow = true;
            body.receiveShadow = false;
        }
        scene.add(car);

        ground = new THREE.Mesh(
            new THREE.PlaneGeometry(20000,20000,200,200),
            ground_material); //new THREE.MeshLambertMaterial({color:0xF5F5DC, side:THREE.DoubleSide}));
        ground.receiveShadow = true;
        ground.castShadow = false;
        scene.add(ground);

        path = new THREE.Object3D();
        for(let i = 0; i < test_path.length; ++i) {
            var node = test_path[i];
            var waypoint = new THREE.Mesh(
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

        var lidar_template = new THREE.Mesh(
            new THREE.BoxGeometry(0.03,0.03,0.5),
            new THREE.MeshLambertMaterial({color: 0x00ff00}));
        this.lidar_elements = [];
        for(let i = 0; i < 10000; i++) {
            var lidar_element = lidar_template.clone();
            var l = Math.random() * 50 + 1;
            lidar_element.position.x = l * Math.cos(i*Math.PI/180.);
            lidar_element.position.y = l * Math.sin(i*Math.PI/180.);
            lidar_element.position.z = 0.25;
            lidar_element.l = l;
            lidar_elements.push(lidar_element)
            scene.add(lidar_element);
        }

        this.stats = new Stats();
        stats.showPanel( 0 ); // 0: fps, 1: ms, 2: mb, 3+: custom
        document.body.appendChild( stats.dom );

        render();
    }

    function render() {
        stats.begin();
        for(let i = 0; i < lidar_elements.length; i++)  {
            var lidar_element = lidar_elements[i];
            lidar_element.l = lidar_element.l - 0.01;
            if(lidar_element.l < 1) {
                lidar_element.l = 50;
            }
            lidar_element.position.x = lidar_element.l * Math.cos(i*Math.PI/180.);
            lidar_element.position.y = lidar_element.l * Math.sin(i*Math.PI/180.);
        }
        stats.end();
        renderer.render(scene, camera);
        requestAnimationFrame(render);  // force to call again at next repaint
    }

    window.onload = initScene;

    return {
        scene : scene
    }


})();
