#include "lidar.h"

int main() {
    //test_lidar();
    //return 0;
    LidarUnit lidar;
    try {
        lidar.run();
        for(auto rpm : {180,349}) {
            lidar.set_rpm(rpm);
            for(int x=0; x < 50; x++) {
                lidar.get_scan();
                cout << lidar.current_scan.display_string() << endl;
                cout << "completed scans: " << lidar.completed_scan_count << endl;
            }
        }
        lidar.stop();
    } catch (string s) {
        cout << "Exception: " << s << endl;
    }

    lidar.stop();
    return 0;
}
