#ifndef FILE_NAMES_H
#define FILE_NAMES_H


#include "system.h"
#include <string>
#include <vector>
#include <algorithm>

class FileNames {
public:
  string tracks_folder;
  FileNames(string tracks_folder = string(getenv("HOME")) + "/car/tracks");

  string get_track_folder(string track_name);

  vector<string> get_track_names();

  string get_routes_folder(string track_name);

  string get_route_folder(string track_name, string route_name);

  vector<string> get_route_names(string track_name);
  vector<string> get_run_names(string track_name, string route_name);

  string get_runs_folder(string track_name, string route_name);


  string get_run_folder(string track_name, string route_name, string run_name);

  string next_route_name(string track_name);

  string recording_file_path(string track_name, string route_name, string run_name = "");

  string commands_file_path(string track_name, string route_name, string run_name = "");


  vector<string> stereo_video_file_paths(string track_name, string route_name, string run_name = "");

  string exception_file_path(string track_name, string route_name, string run_name = "");

  string config_file_path(string track_name, string route_name, string run_name = "");

  string path_file_path(string track_name, string route_name, string run_name = "");

  string next_run_name(string track_name, string route_name);

  string planned_path_file_path(string track_name, string route_name, string run_name);
  string dynamics_file_path(string track_name, string route_name, string run_name = "");
private:
  string get_folder( string track_name, string route_name, string run_name = "");
};


#endif // FILE_NAMES_H
