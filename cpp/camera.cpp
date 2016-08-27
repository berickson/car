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
#include <chrono>

using namespace std;


double Camera::get_fps()
{
  return fps;
}

Camera::Camera()
{
}

Camera::~Camera()
{
  // shut down anything that may or may not have started
  end_capture_movie();
  if(cap.isOpened())
    cap.release();

}

void Camera::set_recording_path(string path)
{
  recording_path = path;
}

void Camera::warm_up()
{
  if(warmed_up) return;

  cap.open(cam_number);
  grabber.begin_grabbing(&cap);
  warmed_up = true;
}

void Camera::prepare_video_writer(string path)
{
  cv::Size frame_size = cv::Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  // int fourcc = (int) cap.get(CV_CAP_PROP_FOURCC);
  int fourcc = CV_FOURCC('M','J','P','G');
  fps = (int) cap.get(CV_CAP_PROP_FPS);
  fps = 10;
  bool is_color = true;
  recording_path = path;



  // get default recording path if none set
  if(recording_path.length()==0) {
    stringstream ss;
    ss << "video" << cam_number << ".avi";
    recording_path = ss.str();
  }

  output_video.open(recording_path , fourcc, fps, frame_size, is_color);
}

void Camera::begin_capture_movie() {
  warm_up();
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

void Camera::release_video_writer()
{
  if(output_video.isOpened())
    output_video.release();
}

bool Camera::get_latest_frame()
{
  if(grabber.get_latest_frame(latest_frame)) {
    cv::flip(latest_frame,latest_frame,-1); // todo: make separate function and accessor
  }

  return grabber.get_frame_count_grabbed() > 0;

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
  prepare_video_writer(recording_path);


  while(record_on.load() == true) {
    if(grabber.get_latest_frame(latest_frame)) {
      write_latest_frame();
    }
    usleep(1000); // 1000 = one ms
  }
  release_video_writer();

  cout << "leaving record thread proc" << endl;
}

void Camera::write_latest_frame() {
  output_video.write(latest_frame);
  ++frame_count_saved;
}


#include <iostream>




StereoCamera::StereoCamera()
{
  left_camera.cam_number = 1;
  right_camera.cam_number = 0;
  cameras.push_back(&left_camera);
  cameras.push_back(&right_camera);
}

void StereoCamera::warm_up()
{
  for(Camera* camera : cameras) {
    camera->warm_up();
  }
}

void StereoCamera::begin_recording(string left_recording_path_, string right_recording_path_)
{
  left_recording_path = left_recording_path_;
  right_recording_path = right_recording_path_;
  warm_up();
  record_on.store(true);
  this->record_thread = std::thread(&StereoCamera::record_thread_proc, this);
}


void StereoCamera::end_recording()
{
  record_on.store(false);
  if(record_thread.joinable()){
    record_thread.join();
  }

}

void StereoCamera::record_thread_proc()
{

  left_camera.prepare_video_writer(left_recording_path);
  right_camera.prepare_video_writer(right_recording_path);

  cv::Mat left_frame;
  cv::Mat right_frame;

  double fps = 10;
  auto t_start = std::chrono::high_resolution_clock::now();
  auto t_next_frame = t_start;
  std::chrono::microseconds us_per_frame((int) (1E6/fps) );

  while(this->record_on.load()) {
    std::this_thread::sleep_until(t_next_frame);
    if(!left_camera.get_latest_frame())
      continue;
    if(!right_camera.get_latest_frame())
      continue;
    left_camera.write_latest_frame();
    right_camera.write_latest_frame();
    t_next_frame += us_per_frame;
    ++frames_recorded;

  }
  left_camera.release_video_writer();
  right_camera.release_video_writer();

}




void test_camera() {


  Camera left;
  Camera right;
  left.cam_number = 1;
  right.cam_number = 0;
  vector<Camera *> cameras;
  cameras.push_back(&left);
  cameras.push_back(&right);

  int seconds_to_grab = 10;
  cout << "grabbing cameras for "<< seconds_to_grab << " seconds" << endl;
  for(Camera * camera: cameras) {camera->begin_capture_movie();}
  for(int i=0;i<seconds_to_grab;++i) {
    usleep(1 * 1E6);
    for(Camera * camera: cameras) {
      cout << "camera " << camera->cam_number
           << " grabbed " << camera->get_frame_count_grabbed()
           << " saved "<< camera->get_frame_count_saved() << " images"
           << endl;
    }
  }
  for(auto camera: cameras) {camera->end_capture_movie();}


}


void test_stereo_camera() {


  StereoCamera camera;
  camera.warm_up();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  int seconds_to_grab = 10;
  cout << "grabbing cameras for "<< seconds_to_grab << " seconds x 2" << endl;
  camera.begin_recording("left.avi","right.avi");

  for(int i=0;i<seconds_to_grab;++i) {
    usleep( 1E6);
    cout << "frames recorded: " << camera.frames_recorded << endl;
  }
  camera.end_recording();


}

