var example = (function(){
    "use strict";

    var scene = new THREE.Scene(),
    renderer = new THREE.WebGLRenderer({antialias:true}),
    light = new THREE.AmbientLight(0x888888),
    sun = new THREE.DirectionalLight(0xffffff,0.75),
    ground,
    camera,
    path,
    car,
    controls;

    renderer.shadowMap.enabled = true;
    renderer.shadowMap.type = THREE.PCFSoftShadowMap; // default THREE.PCFShadowMa

    //Set up shadow properties for the light
    sun.shadow.mapSize.width = 5512;  // default
    sun.shadow.mapSize.height = 5512; // default
    sun.shadow.camera.near = 0.5;    // default
    sun.shadow.camera.far = 5500;     // default

    sun.position.z = 1000; // shine down from z
    sun.castShadow = true;

    function initScene(){
        renderer.setSize(window.innerWidth, window.innerHeight);
        document.getElementById("webgl-container").appendChild(renderer.domElement);
        scene.add(light);
        scene.add(sun);
        //scene.add(new THREE.GridHelper(1000,1000,0x555555,0x005500));
        camera = new THREE.PerspectiveCamera(
            35, // FOV, degrees
            window.innerWidth / window.innerHeight, // aspect ratio
            .1,   // near cutoff
            1000 // far cutoff
        );
        camera.position.z = 2;
        camera.position.y = -0;
        camera.position.x = -5;
        camera.up = new THREE.Vector3(0,0,1);
//        camera.lookAt(0,0,0);
//        camera.rotation.x = - Math.PI/2;

        controls = new THREE.OrbitControls( camera, renderer.domElement );
        controls.target.set( 0, 0, 0 ); // view direction perpendicular to XY-plane
        controls.enableRotate = true;
        controls.enableZoom = true; // optional


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
            edges.material.linewidth = 2;
            car.add(edges);
            car.position.z = h/2 + 0.03;
            car.position.x = -l/2;
            body.castShadow = true;
            body.receiveShadow = false;
        }
        scene.add(car);
        

        

        ground = new THREE.Mesh(
            new THREE.PlaneGeometry(1000,1000),
            new THREE.MeshLambertMaterial({color:0xF5F5DC, side:THREE.DoubleSide}));
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
        //path.rotateOnAxis(new THREE.Vector3(1,0,0), Math.PI/2);
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