#include "system.h"
#include <string>
#include <vector>
#include <algorithm>

class FileNames {
public:
  string tracks_folder;
  FileNames(string tracks_folder = "../../tracks") {
    this->tracks_folder = tracks_folder;
  }

  inline string get_track_folder(string track_name) {
    return path_join(tracks_folder, track_name);
  }

  inline vector<string> get_track_names() {
    return child_folders(tracks_folder);
  }

  inline string get_routes_folder(string track_name) {
    return path_join(get_track_folder(track_name),"routes");
  }

  inline string get_route_folder(string track_name, string route_name) {
    return path_join(get_routes_folder(track_name),route_name);
  }

  inline vector<string> get_route_names(string track_name) {
    return child_folders(get_routes_folder(track_name));
  }

  inline string get_runs_folder(string track_name, string route_name) {
    return path_join(get_route_folder(track_name,route_name),"runs");
  }


  string get_run_folder(string track_name, string route_name, string run_name) {
    return path_join(get_runs_folder(track_name,route_name),run_name);
  }

  string next_route_name(string track_name){
    for(char c = 'A';c<='Z';c++) {
      string route_name;
      route_name.push_back(c);
      if(file_exists(get_route_folder(track_name,route_name)))
        continue;
      return route_name;
    }
   throw string("could not find empty route");
  }

  string recording_file_path(string track_name, string route_name, string run_name = ""){
    return path_join(get_folder(track_name, route_name, run_name),"recording.csv");
  }

  string commands_file_path(string track_name, string route_name, string run_name = ""){
    return path_join(get_folder(track_name, route_name, run_name),"commands.csv");
  }


  vector<string> stereo_video_file_paths(string track_name, string route_name, string run_name) {
    string f = get_folder(track_name, route_name, run_name);
    vector<string> rv;
    rv.push_back(path_join(f,"video_left.avi"));
    rv.push_back(path_join(f,"video_right.avi"));
    return rv;
  }

  string exception_file_path(string track_name, string route_name, string run_name = ""){
    return path_join(get_folder(track_name, route_name, run_name),"exceptions.log");
  }

  string config_file_path(string track_name, string route_name, string run_name = ""){
    return path_join(get_folder(track_name, route_name, run_name),"config.json");
  }

  string path_file_path(string track_name, string route_name, string run_name = ""){
    return path_join(get_folder(track_name, route_name, run_name),"path.csv");
  }

  string next_run_name(string track_name, string route_name) {
    for( int i=0; i<999; ++i) {
      string run_name = to_string(i);
      if (file_exists(get_run_folder(track_name, route_name, run_name)))
        continue;
      return run_name;
    }
    throw string("could not find empty route");
  }

private:
  string get_folder( string track_name, string route_name, string run_name = "") {
    if(run_name.size()==0) {
       return get_route_folder(track_name,route_name);
    } else {
      return get_run_folder(track_name, route_name, run_name);
    }
  }
};
