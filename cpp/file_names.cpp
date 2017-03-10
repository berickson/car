#include "file_names.h"

FileNames::FileNames(string tracks_folder) {
  this->tracks_folder = tracks_folder;
}

string FileNames::get_track_folder(string track_name) {
  return path_join(tracks_folder, track_name);
}

vector<string> FileNames::get_track_names() {
  return child_folders(tracks_folder);
}

string FileNames::get_routes_folder(string track_name) {
  return path_join(get_track_folder(track_name),"routes");
}

string FileNames::get_route_folder(string track_name, string route_name) {
  return path_join(get_routes_folder(track_name),route_name);
}

vector<string> FileNames::get_route_names(string track_name) {
  return child_folders(get_routes_folder(track_name));
}

vector<string> FileNames::get_run_names(string track_name, string route_name)
{
  return child_folders(get_runs_folder(track_name, route_name));
}

string FileNames::get_runs_folder(string track_name, string route_name) {
  return path_join(get_route_folder(track_name,route_name),"runs");
}

string FileNames::get_run_folder(string track_name, string route_name, string run_name) {
  return path_join(get_runs_folder(track_name,route_name),run_name);
}

string FileNames::next_route_name(string track_name){
  for(char c = 'A';c<='Z';c++) {
    string route_name;
    route_name.push_back(c);
    if(file_exists(get_route_folder(track_name,route_name)))
      continue;
    return route_name;
  }
  throw string("could not find empty route");
}

string FileNames::recording_file_path(string track_name, string route_name, string run_name){
  return path_join(get_folder(track_name, route_name, run_name),"recording.csv");
}

string FileNames::dynamics_file_path(string track_name, string route_name, string run_name) {
  return path_join(get_folder(track_name, route_name, run_name),"td.csv");
}

string FileNames::state_log_path(string track_name, string route_name, string run_name) {
  return path_join(get_folder(track_name, route_name, run_name),"state.csv");
}

string FileNames::commands_file_path(string track_name, string route_name, string run_name){
  return path_join(get_folder(track_name, route_name, run_name),"commands.csv");
}

vector<string> FileNames::stereo_video_file_paths(string track_name, string route_name, string run_name) {
  string f = get_folder(track_name, route_name, run_name);
  vector<string> rv;
  rv.push_back(path_join(f,"video_left.avi"));
  rv.push_back(path_join(f,"video_right.avi"));
  return rv;
}

string FileNames::exception_file_path(string track_name, string route_name, string run_name){
  return path_join(get_folder(track_name, route_name, run_name),"exceptions.log");
}

string FileNames::config_file_path(string track_name, string route_name, string run_name){
  return path_join(get_folder(track_name, route_name, run_name),"config.json");
}

string FileNames::planned_path_file_path(string track_name, string route_name, string run_name){
  return path_join(get_folder(track_name, route_name, run_name),"planned_path.csv");
}

string FileNames::path_file_path(string track_name, string route_name, string run_name){
  return path_join(get_folder(track_name, route_name, run_name),"path.csv");
}

string FileNames::next_run_name(string track_name, string route_name) {
  for( int i=0; i<999; ++i) {
    string run_name = to_string(i);
    if (file_exists(get_run_folder(track_name, route_name, run_name)))
      continue;
    return run_name;
  }
  throw string("could not find empty route");
}

string FileNames::get_folder(string track_name, string route_name, string run_name) {
  if(run_name.size()==0) {
    return get_route_folder(track_name,route_name);
  } else {
    return get_run_folder(track_name, route_name, run_name);
  }
}
