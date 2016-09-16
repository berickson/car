#include "run_settings.h"
#include "file_names.h"
#include "iostream"
#include "split.h"
#include "trim.h"


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
       << "steering_k_p = " << steering_k_p << endl
       << "steering_k_i = " << steering_k_i << endl
       << "steering_k_d = " << steering_k_d << endl

       << "v_k_p = " << v_k_p << endl
       << "v_k_i = " << v_k_i << endl
       << "v_k_d = " << v_k_d << endl

       << "slip_rate = " << slip_rate << endl
       << "slip_slop = " << slip_slop << endl

       << "t_ahead = " << t_ahead << endl
       << "d_ahead = " << d_ahead << endl
       << "k_smooth = " << k_smooth << endl

       << "prune_max = " << prune_max << endl
       << "prune_tolerance = " << prune_tolerance << endl
       << "capture_video = " << capture_video << endl
       << "crash_recovery = " << crash_recovery << endl
       << "optimize_velocity = " << optimize_velocity <<endl;

}

void RunSettings::load_from_file(string path)
{
  ifstream file;
  file.open(path,ios_base::in);
  if(!file.is_open()) {
    throw (string) "could not open " + path;
  }

  std::string line;
  while(!file.eof()) {
    std::getline(file,line);
    vector<string> u = split(line,'=');
    if(u.size()!=2) continue;
    string name = trimmed(u[0]);
    string value = trimmed(u[1]);


    if(name == "track_name")
      this->track_name = value;
    else if (name == "route_name")
      this->route_name = value;
    else if (name == "max_a")
      this->max_a = stod(value);
    else if (name == "max_v")
      this->max_v = stod(value);
    else if (name == "d_ahead")
      this->d_ahead = stod(value);
    else if (name == "t_ahead")
      this->t_ahead = stod(value);
    else if (name == "k_p")
      this->steering_k_p = stod(value);
    else if (name == "k_d")
      this->steering_k_d = stod(value);
    else if (name == "steering_k_p")
      this->steering_k_p = stod(value);
    else if (name == "steering_k_i")
      this->steering_k_i = stod(value);
    else if (name == "steering_k_d")
      this->steering_k_d = stod(value);

    else if (name == "v_k_p")
      this->steering_k_d = stod(value);
    else if (name == "v_k_i")
      this->steering_k_i = stod(value);
    else if (name == "v_k_d")
      this->steering_k_d = stod(value);
    else if (name == "prune_max")
      this->prune_max = stod(value);
    else if (name == "prune_tolerance")
      this->prune_tolerance = stod(value);
    else if (name == "k_smooth")
      this->k_smooth = stod(value);
    else if (name == "capture_video")
      this->capture_video = stod(value)>0;
    else if (name == "crash_recovery")
      this->crash_recovery = stod(value)>0;
    else if (name == "optimize_velocity")
      this->optimize_velocity= stod(value)>0;

  }
}
