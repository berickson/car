#include "run_settings.h"
#include "filenames.h"


RunSettings::RunSettings(){
  auto f = FileNames();
  auto track_names = f.get_track_names();
  if(track_names.size() == 0) {
    track_name = "";
    route_name = "";
    return;
  }
  track_name = track_names[0];
  auto route_names = f.get_route_names(track_name);
  if(route_names.size()==0) {
    route_name = "";
    return;
  }
  route_name = route_names[0];
}

void RunSettings::write_to_file(string path) {
  fstream file;
  file.open(path,ios_base::out);
  if(!file.is_open()) {
    throw (string) "could not open " + path;

  }
  file << "track_name = " << track_name << endl
       << "route_name = " << route_name << endl
       << "max_a = " << max_a << endl
       << "max_v = " << max_v << endl
       << "t_ahead = " << t_ahead << endl
       << "d_ahead = " << d_ahead << endl
       << "k_smooth = " << k_smooth << endl
       << "capture_video = " << capture_video << endl;

}
