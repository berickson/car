#pragma once
#include "../usb.h"
#include "../string_utils.h"
#include "../geometry.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <queue>

using namespace std;


struct LidarMeasurement {
    Angle angle;
    double distance_meters = NAN;
    double signal_strength = NAN;
    enum measure_status {ok, crc_error, invalid_data, low_signal, uninitialized} status = measure_status::uninitialized;

    string display_string();
};

struct LidarScan {
    vector<LidarMeasurement> measurements;

    LidarScan();

    string display_string();
};

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

    bool get_scan();

    void run();

    void stop();
    bool try_get_scan(int ms_to_wait);
};


