angular.module("car",[]).controller("CarController", function($scope, $http, $timeout,$log) {
  var vm = this;
  vm.Math = Math;
  vm.JSON = JSON;

  vm.type_of = function (val) {
    return typeof (val);
  };

  vm.is_changed = function (field) {
    return field.original_value.toString() !== field.value.toString();

  };

  vm.to_field_array = function (x) {
    var rv = [];
    for (var k in x) {
      rv.push({
        key: k,
        value: x[k],
        original_value: x[k],
        data_type: typeof (x[k])
      });
    }
    return rv;
  };

  vm.set_changes_from_field_array = function (o, field_array) {
    for (var i in field_array) {
      var field = field_array[i];
      if (vm.is_changed(field)) {
        if (field.data_type == 'number') {
          o[field.key] = Number(field.value);
        } else if (field.data_type == 'boolean') {
          o[field.key] = Boolean(field.value);
        } else {
          $log.error("unknown data type" + field.data_type);
        }
      }
    }
  };


  vm.poweroff = function () {
    vm.go_error = "";
    $http.put('/pi/poweroff', "1").success(function () {
      $log.info('poweroff success');
    }).error(function (response, code) {
      vm.go_error = "  (" + code + ")" + response.message;
    });
    $log.info("poweroff clicked");
  };


  vm.save_run_settings = function () {
    vm.go_error = "";
    vm.set_changes_from_field_array(vm.run_settings, vm.run_settings_array);
    var req = {
      url: '/run_settings',
      method: 'PUT',
      data: angular.toJson(vm.run_settings),
      headers: {
        'Content-Type': 'application/json'
      }
    };

    var req2 = {
      url: vm.route_path_url(),
      method: 'PUT',
      data: angular.toJson(vm.route_path),
      headers: {
        'Content-Type': 'application/json'
      }
    };

    $http(req).success(function () {
      $log.info('save run settings success');
      $http(req2).success(function () {
        $log.info('save route path success');
      }).error(function (response, code) {
        $log.warn("failed to save route path");
        $log.warn("  (" + code + ")" + response);
      });
    }).error(function (response, code) {
      vm.go_error = "  (" + code + ")" + response;
    });

    $log.info("save_run_settings clicked");

  };


  vm.go = function () {
    vm.go_error = "";
    $http.put('/command/go', "1").success(function () {
      $log.info('go success');
    }).error(function (response, code) {
      vm.go_error = "  (" + code + ")" + response.message;
    });
    $log.info("go clicked");
  };

  vm.stop = function () {
    $http.put('/command/stop', "1").success(function () {
      $log.info('stop success');
    }).error(function (response, code) {
      vm.stop_error = "  (" + code + ")" + response.message;
    });
    $log.info("stop clicked");
  };

  vm.record = function () {
    $http.put('/command/record', "1").success(function () {
      $log.info('record success');
    }).error(function (response, code) {
      vm.record_error = "  (" + code + ")" + response.message;
    });
    $log.info("record clicked");
  };

  vm.reset_odometer = function () {
    $http.put('/command/reset_odometer', "1").success(function () {
      $log.info('reset_odometer success');
    }).error(function (response, code) {
      vm.reset_odometer_error = "  (" + code + ")" + response.message;
    });
    $log.info("reset_odometer clicked");
  };

  vm.reset_zoom = function () {
    vm.viewbox_left = vm.route_path_min_x - 0.7;
    vm.viewbox_top = -vm.route_path_max_y;
    vm.viewbox_width = vm.route_path_width + 0.8;
    vm.viewbox_height = vm.route_path_height;
  };


  vm.pan_delta = 0.1;
  vm.zoom_ratio = Math.sqrt(2);

  vm.zoom_in = function () {
    var center_x = vm.viewbox_left + vm.viewbox_width / 2.0;
    var center_y = vm.viewbox_top + vm.viewbox_height / 2.0;

    vm.viewbox_height /= vm.zoom_ratio;
    vm.viewbox_width /= vm.zoom_ratio;

    vm.view_box_top = center_y - vm.viewbox_height / 2;
    vm.view_box_left = center_x - vm.viewbox_width / 2;
  };

  vm.zoom_out = function () {
    var center_x = vm.viewbox_left + vm.viewbox_width / 2.0;
    var center_y = vm.viewbox_top + vm.viewbox_height / 2.0;

    vm.viewbox_height *= vm.zoom_ratio;
    vm.viewbox_width *= vm.zoom_ratio;

    vm.view_box_top = center_y - vm.viewbox_height / 2;
    vm.view_box_left = center_x - vm.viewbox_width / 2;
  };

  vm.pan_left = function () {
    vm.viewbox_left += vm.viewbox_width * vm.pan_delta;
  };

  vm.pan_right = function () {
    vm.viewbox_left -= vm.viewbox_width * vm.pan_delta;
  };

  vm.pan_up = function () {
    vm.viewbox_top += vm.viewbox_height * vm.pan_delta;
  };

  vm.pan_down = function () {
    vm.viewbox_top -= vm.viewbox_height * vm.pan_delta;
  };


  $scope.$watch("car.run_settings.track_name", function () {
    if (vm.run_settings && vm.run_settings.track_name !== null && vm.run_settings.track_name.length > 0) {
      $log.info("selected track:", vm.run_settings.track_name);
      $http.get('/tracks/' + vm.run_settings.track_name + "/route_names").
        success(function (result /*, status, headers, config*/) {
          vm.route_names = result.route_names;
        }).error(function (/*data, status, headers, config*/) {
          vm.route_names = ['n/a'];
          // log error
        });
    }
  });

  vm.route_path_url = function () {
    return '/tracks/' + vm.run_settings.track_name + "/routes/" + vm.run_settings.route_name + "/path";
  };

  vm.has_road_sign = function(node) {
    if ( angular.isDefined(node.road_sign_label) && node.road_sign_label && node.road_sign_label.length > 0 ){
      return true;
    }
    if ( angular.isDefined(node.road_sign_command) && node.road_sign_command && node.road_sign_command.length > 0 ) {
      return true;
    }
    return false;
  };

  vm.populate_road_sign_nodes = function () {
    vm.road_sign_nodes = [];
    for (var i in vm.route_path) {
      var node = vm.route_path[i];
      if (vm.has_road_sign(node)) {
        vm.road_sign_nodes.push(node);
      }
    }
  };

  $scope.$watch("car.run_settings.route_name", function () {
    if (vm.run_settings && vm.run_settings.route_name !== null && vm.run_settings.route_name.length > 0) {
      $http.get(vm.route_path_url()).
        success(function (route_path /*, status, headers, config*/) {
          vm.route_path = route_path;
          var route_x = route_path.map(function (v) { return v.x; });
          var route_y = route_path.map(function (v) { return v.y; });

          vm.route_path_min_x = Math.min.apply(null, route_x);
          vm.route_path_min_y = Math.min.apply(null, route_y);
          vm.route_path_max_x = Math.max.apply(null, route_x);
          vm.route_path_max_y = Math.max.apply(null, route_y);
          vm.route_path_width = vm.route_path_max_x - vm.route_path_min_x;
          vm.route_path_height = vm.route_path_max_y - vm.route_path_min_y;
          vm.reset_zoom();

          vm.populate_road_sign_nodes();

        }).error(function (/*data, status, headers, config*/) {
          vm.route_names = ['n/a'];
          $log.error('$http failed to get route path');
        });
    }
  });


  vm.track_names = [];
  vm.track_name = "";
  $http.get('/track_names').
    success(function ( result) {
      vm.track_names = result.track_names;
    }).
    error(function () {
      $log.error('$http failed to get track names');
    });
  vm.poll_ok = false;
  vm.display_car_state = false;


  var poller = function () {
    $http({ method: 'GET', timeout: 5000, url: '/car/get_state' })
      .then(function (r) {
        vm.car_state = r.data;
        var v_bat = vm.car_state.v_bat;
        var min_bat = 3.5 * 3;
        var max_bat = 4.2 * 3;
        vm.battery_percent = 100 * (v_bat - min_bat) / (max_bat - min_bat);
        vm.poll_ok = true;
        $timeout(poller, 100);
      },
      function () {
        vm.poll_ok = false;
        $timeout(poller, 1000);
      });
  };
  poller();

  vm.road_sign_nodes = [];

  vm.rjust = function (s, width) {
    if (angular.isUndefined(s)) {
      return " ".repeat(width);
    }
    s = s.toString();
    return " ".repeat(width - s.length) + s;
  };

  vm.property_list = function (o) {
    var s = [];
    for (var property_name in o) {
      s.push(property_name);
    }
    return s.join(", ");
  };

  vm.delete_road_sign = function (road_sign_index) {
    var node = vm.road_sign_nodes[road_sign_index];
    node.road_sign_label = "";
    node.road_sign_command = "";
    node.arg1 = "";
    node.arg2 = "";
    node.arg3 = "";
    vm.populate_road_sign_nodes();
  };

  vm.node_clicked = function ($event, node) {
    vm.selected_node = node;
    if (!vm.has_road_sign(node)) {
      node.road_sign_label = vm.road_sign_nodes.length.toString();
      node.road_sign_command = "stop";
      vm.populate_road_sign_nodes();
    }
    return;
  };

  var poller2 = function () {
    $http({ method: 'GET', timeout: 5000, url: '/pi/get_state' })
      .then(function (r) {
        vm.pi_state = r.data;
        $timeout(poller2, 1000);
      },
      function () {
        $timeout(poller2, 1000);
      });
  };
  poller2();

  var poller3 = function () {
    $http({ method: 'GET', timeout: 5000, url: '/run_settings' })
      .then(function (r) {
        vm.run_settings = r.data;
        vm.run_settings_array = vm.to_field_array(vm.run_settings);
      },
      function () {
        $timeout(poller3, 1000);
      });
  };
  poller3();
});
