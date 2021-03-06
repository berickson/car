#pragma once
#include "../usb.h"
#include "../string-utils.h"
#include "../geometry.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>
#include <cstdint>
#include <eigen3/Eigen/Dense>

using namespace std;


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

    vector<LidarMeasurement> measurements;
    vector<ScanSegment> find_lines(double tolerance, int min_point_count = 8);

    LidarScan();

    string display_string();
};

// contrls lidar based on 
// https://github.com/getSurreal/XV_Lidar_Controller
class LidarUnit {
public:
    Usb usb2;
    WorkQueue<LidarMeasurement> measurement_queue;
    LidarScan scan1;
    LidarScan scan2;
    LidarScan & current_scan = scan1;
    LidarScan & next_scan = scan2;
    WorkQueue<string> usb_queue;
    int completed_scan_count = 0;

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
