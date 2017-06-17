#include "lidar.h"

std::__cxx11::string LidarMeasurement::display_string() {
    stringstream s;
    s << angle.to_string() << " ";
    if(status == measure_status::ok) {
        s << distance_meters << "m strength: " << signal_strength;
    }
    return s.str();
}

LidarScan::LidarScan() {
    measurements.resize(360);
}

std::__cxx11::string LidarScan::display_string() {
    stringstream s;
    for(LidarMeasurement & m : measurements) {
        s <<  m.display_string() << endl;
    }
    return s.str();
}

bool LidarUnit::try_get_scan(int ms_to_wait = 5000)
{
    string l;
    while(true) {
        if(! usb_queue.try_pop(l, ms_to_wait)) {
            return false;
        }
        trim(l);
        vector<string> fields = split(l);
        // see if this is an angle measurment
        if(fields.size() > 2 && fields[1] == "A") {
            LidarMeasurement m;
            int degrees = atoi(fields[2].c_str());
            m.angle.set_degrees(degrees);
            string v = fields[3];
            if(v=="S") {
                m.status = LidarMeasurement::measure_status::low_signal;
            } else if (v=="I") {
                m.status = LidarMeasurement::measure_status::invalid_data;
            } else if (v=="CRC") {
                m.status = LidarMeasurement::measure_status::crc_error;
            } else {
                m.distance_meters = atoi(v.c_str())/1000.;
                m.signal_strength = atoi(fields[4].c_str());
                m.status = LidarMeasurement::measure_status::ok;
            }
            next_scan.measurements[degrees] = m;
            if(degrees == 359) {
                swap(current_scan, next_scan);
                completed_scan_count++;
                return true;
            }
        }
    }
    return false;
}

bool LidarUnit::get_scan() {
    try_get_scan();
    return true;
}

void LidarUnit::run() {
    usb2.write_on_connect("");
    usb2.run();
    usb2.write_line("ResetConfig");
    usb2.write_line("HideRaw");
    usb2.write_line("HideAll");
    usb2.write_line("SetSampleTime 40");
    usb2.write_line("ShowAll");
    //usb2.write_line("SetAngle 0, 15-30, 45-50, 10 ");
    usb2.write_line("ShowAll");

    usb2.add_line_listener(&usb_queue);
    //while(true) {
    //            get_scan();
    //        }
}

void LidarUnit::stop() {
    usb2.stop();
}
