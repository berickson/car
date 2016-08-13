#include "camera.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
#include "geometry.h"

#include <opencv2/core/core.hpp>
#include "opencv2/videoio.hpp"
#include <thread>
#include <unistd.h> // usleep

#include <iostream>

using namespace std;


Camera::Camera()
{
}

void Camera::set_recording_path(string path)
{
  recording_path = path;
}

void Camera::begin_capture_movie() {
  cap.open(cam_number);
  grabber.begin_grabbing(&cap);
  record_on.store(true);
  this->record_thread = std::thread(&Camera::record_thread_proc, this);

}

void Camera::end_capture_movie() {
  record_on.store(false);
  if(record_thread.joinable()){
    record_thread.join();
  }
  grabber.end_grabbing();
}

int Camera::get_frame_count_grabbed()
{
  return grabber.frames_grabbed;
}

int Camera::get_frame_count_saved()
{
  return this->frame_count_saved;
}

void Camera::record_thread_proc() {
  cout << "inside record thread proc" << endl;

  cv::Size frame_size = cv::Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  int fourcc = (int) cap.get(CV_CAP_PROP_FOURCC);
  int fps = (int) cap.get(CV_CAP_PROP_FPS);
  bool is_color = true;


  cv::VideoWriter output_video;

  // get default recording path if none set
  if(recording_path.length()==0) {
    stringstream ss;
    ss << "video" << cam_number << ".avi";
    recording_path = ss.str();
  }

  output_video.open(recording_path , fourcc, fps, frame_size, is_color);

  while(record_on.load() == true) {
    if(grabber.get_latest_frame(latest_frame)) {
      output_video.write(latest_frame);
      ++frame_count_saved;
    }
    usleep(1000); // 1000 = one ms
  }

  cout << "leaving record thread proc" << endl;
}


#include <iostream>

void test_camera() {


  Camera left;
  Camera right;
  left.cam_number = 1;
  right.cam_number = 0;
  vector<Camera *> cameras;
  cameras.push_back(&left);
  cameras.push_back(&right);

  int seconds_to_grab = 10;
  cout << "grabbing cameras for "<< seconds_to_grab << " seconds x 2" << endl;
  for(Camera * camera: cameras) {camera->begin_capture_movie();}
  for(int i=0;i<seconds_to_grab;++i) {
    usleep(2 * 1E6);
    for(Camera * camera: cameras) {
      cout << "camera " << camera->cam_number
           << " grabbed " << camera->get_frame_count_grabbed()
           << " saved "<< camera->get_frame_count_saved() << " images"
           << endl;
    }
  }
  for(auto camera: cameras) {camera->end_capture_movie();}


}

