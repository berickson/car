var example = (function(){
    "use strict";

    var scene = new THREE.Scene(),
    renderer = new THREE.WebGLRenderer({antialias:true}),
    light = new THREE.AmbientLight(0x888888),
    sun = new THREE.DirectionalLight(0xffffff,0.75),
    camera,
    box,
    controls;

    function initScene(){
        renderer.setSize(window.innerWidth, window.innerHeight);
        document.getElementById("webgl-container").appendChild(renderer.domElement);
        scene.add(light);
        scene.add(sun);
        scene.add(new THREE.GridHelper(1000,1000,0x555555,0x005500));
        camera = new THREE.PerspectiveCamera(
            35, // FOV, degrees
            window.innerWidth / window.innerHeight, // aspect ratio
            .1,   // near cutoff
            1000 // far cutoff
        );
        camera.position.z = 10;
        camera.position.y = 10;
        camera.rotation.x = -0.5;

        controls = new THREE.OrbitControls( camera, renderer.domElement );
        controls.target.set( 0, 0, 0 ); // view direction perpendicular to XY-plane
        //controls.enableRotate = false;
        controls.enableZoom = true; // optional

        scene.add(camera);
        box = new THREE.Mesh(
            new THREE.BoxGeometry(2,2,2),
            new THREE.MeshLambertMaterial({color: 0xFFFF00, transparent:true, opacity:0.7, side:THREE.DoubleSide})
            //new THREE.MeshBasicMaterial({color: 0xFF0000})
        );
        box.position.y = 2;
        box.name = "box";
        scene.add(box);

        render();
    }

    function render() {
        box.rotation.y += 0.01; // radians
        box.rotation.x += 0.005; // radians
        renderer.render(scene, camera);
        requestAnimationFrame(render);  // force to call again at next repaint
    }

    window.onload = initScene;

    return {
        scene : scene
    }


})();