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

    ground_texture = THREE.ImageUtils.loadTexture("TexturesCom_WoodPlanksBare0498_5_seamless_S.png");
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

    var sky_texture = THREE.ImageUtils.loadTexture("TexturesCom_Skies0306_M.jpg");
    var sky_geometry =  new THREE.SphereGeometry(100000,25,25);
    var sky = new THREE.Mesh(sky_geometry,
              new THREE.MeshBasicMaterial({map:sky_texture}));
    sky.material.side = THREE.BackSide;
    scene.add(sky);


    var grid = new THREE.GridHelper( 200, 200, 0x888888, 0xdddddd );
    grid.geometry.rotateX( Math.PI / 2 );
    grid.position.z = 0.001;
    //scene.add(grid);

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
            var edges = new THREE.EdgesHelper( body, 0x000000);
            edges.material.linewidth = 1;
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
                new THREE.MeshLambertMaterial({color: 0x0000FF, transparent:false, opacity:1, side:THREE.DoubleSide})
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

        render();
    }

    function render() {
        renderer.render(scene, camera);
        requestAnimationFrame(render);  // force to call again at next repaint
    }

    window.onload = initScene;

    return {
        scene : scene
    }


})();
