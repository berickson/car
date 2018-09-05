#include "lidar.h"

int main() {
    //test_lidar();
    //return 0;
    LidarUnit lidar;
    try {
        cout << "before run" << endl;
        lidar.run();
        cout << "after run" << endl;
        for(auto rpm : {180,349}) {
            cout << "before set rpm" << endl;
            lidar.set_rpm(rpm);
            cout << "set rpm" << endl;
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
