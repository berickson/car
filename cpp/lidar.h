#pragma once
#include "usb.h"
#include "string_utils.h"
#include "geometry.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <cstdint>
#include "eigen3/Eigen/Dense"
#include "json.hpp"

using namespace std;

struct Pose2dSimple {
    float x = NAN;
    float y = NAN;
    float theta = NAN;
};




struct LidarMeasurement {
    Angle angle;
    double distance_meters = NAN;
    double signal_strength = NAN;
    enum measure_status {ok, crc_error, invalid_data, low_signal, uninitialized} status = measure_status::uninitialized;
    Eigen::Vector2f get_point() {
      return Eigen::Vector2f(distance_meters * cos(angle.radians()),
                             distance_meters * sin(angle.radians()));
    }

    string display_string();
};

struct LidarScan {
    struct ScanSegment {
      int begin_index;
      int end_index;
      Eigen::Vector2f p1;
      Eigen::Vector2f p2;
    };

    int scan_number = 0;
    vector<Pose2dSimple> poses;
    vector<LidarMeasurement> measurements;
    vector<ScanSegment> find_lines(double tolerance, int min_point_count = 8);

    LidarScan();

    string display_string();
    nlohmann::json get_json();
};

// contrls lidar based on 
// https://github.com/getSurreal/XV_Lidar_Controller
class LidarUnit {
public:
    Usb usb2;
    WorkQueue<LidarMeasurement> measurement_queue;
    Pose2dSimple pose;
    LidarScan scan1;
    LidarScan scan2;
    LidarScan & current_scan = scan1;
    LidarScan & next_scan = scan2;
    WorkQueue<StampedString> usb_queue;
    int completed_scan_count = 0;
    bool is_running = true;

    void set_pose(float x, float y, float theta);
    void set_rpm(int rpm);
    void motor_on();
    void motor_off();

    string get_scan_csv_header();
    string get_scan_csv();
    bool get_scan();

    void run();

    void stop();
    bool try_get_scan(int ms_to_wait);
};

struct Corner {
  Eigen::Vector2f p;
};

vector<Corner> find_corners(const vector<LidarScan::ScanSegment> &walls);


void test_lidar();
