#include "lidar.h"

int main() {
    LidarUnit lidar;
    try {
        lidar.run();
        while(true) {
            lidar.get_scan();
            cout << lidar.current_scan.display_string() << endl;
            cout << "completed scans: " << lidar.completed_scan_count << endl;


        }
    } catch (string s) {
        cout << "Exception: " << s << endl;
    }

    lidar.stop();
    return 0;
}
