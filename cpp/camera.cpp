#include "camera.h"
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include "geometry.h"

#include <opencv2/core/core.hpp>
#include "opencv2/videoio.hpp"
#include <thread>
#include <unistd.h> // usleep



using namespace std;


Camera::Camera()
{
}

void Camera::begin_capture_movie() {
  quit = false;
  grab_thread = thread(&Camera::grab_thread_proc, this);
}

void Camera::end_capture_movie() {
  quit = true;
  if(grab_thread.joinable())
    grab_thread.join();
}



void Camera::grab_thread_proc()
{
  try {
    frame_count = 0;
    cv::VideoCapture cap;

    //if(mode == Mode::cap_320_by_240_by_30fps) {
      cap.set(cv::CAP_PROP_FRAME_WIDTH,320);
      cap.set(cv::CAP_PROP_FRAME_HEIGHT,240);
      cap.set(cv::CAP_PROP_FPS,1);
    //}
    if(!cap.open(cam_number)) throw (string) "couldn't open camera";

    cv::Mat frame;
    while(!quit) {
      if(!cap.read(frame)) throw (string) "couldn't grab a frame";
      ++frame_count;

    }
  } catch (string s) {
    grab_thread_error_string = s;
  }

}


#include <iostream>

void test_camera() {


  Camera left;
  Camera right;
  left.cam_number = 1;
  right.cam_number = 0;
  vector<Camera *> cameras;
  //cameras.push_back(&left);
  cameras.push_back(&right);

  cout << "grabbing cameras for 2 seconds x 2" << endl;
  for(Camera * camera: cameras) {camera->begin_capture_movie();}
  for(int i=0;i<2;++i) {
    usleep(2 * 1E6);
    for(auto camera: cameras) {cout << "camera " << camera->cam_number<< " grabbed " << camera->frame_count << " images" << endl;}
  }
  for(auto camera: cameras) {camera->end_capture_movie();}


}

