#include "camera.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
#include "geometry.h"
#include "logger.h"

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
  
  /*
  v4l2-ctl --list-formats-ext --device=/dev/video1
 
  Below is for stereo camera

	Index       : 0
	Type        : Video Capture
	Pixel Format: 'YUYV'
	Name        : YUYV 4:2:2
		Size: Discrete 1280x720
			Interval: Discrete 0.100s (10.000 fps)
			Interval: Discrete 0.200s (5.000 fps)
		Size: Discrete 800x600
			Interval: Discrete 0.067s (15.000 fps)
		Size: Discrete 640x360
			Interval: Discrete 0.032s (31.000 fps)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.067s (15.000 fps)
		Size: Discrete 352x288
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.067s (15.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.067s (15.000 fps)

	Index       : 1
	Type        : Video Capture
	Pixel Format: 'MJPG' (compressed)
	Name        : Motion-JPEG
		Size: Discrete 1280x720
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.040s (25.000 fps)
			Interval: Discrete 0.067s (15.000 fps)
		Size: Discrete 800x600
			Interval: Discrete 0.033s (30.000 fps)
		Size: Discrete 640x360
			Interval: Discrete 0.017s (60.000 fps)
		Size: Discrete 640x480
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.067s (15.000 fps)
		Size: Discrete 352x288
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.067s (15.000 fps)
		Size: Discrete 320x240
			Interval: Discrete 0.033s (30.000 fps)
			Interval: Discrete 0.067s (15.000 fps)
  
  */
  
  cap.set(CV_CAP_PROP_FRAME_WIDTH,320);
  cap.set(CV_CAP_PROP_FRAME_HEIGHT,240);
  cap.set(CV_CAP_PROP_FPS,15);
  cap.set(CV_CAP_PROP_FOURCC, CV_FOURCC('Y','U','Y','V'));

  grabber.begin_grabbing(&cap, to_string(cam_number));
  warmed_up = true;
  log_info((string)"warmed up"+to_string(cam_number));
  
}

void Camera::prepare_video_writer(string path)
{
  cv::Size frame_size = cv::Size((int) cap.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                (int) cap.get(CV_CAP_PROP_FRAME_HEIGHT));
  // int fourcc = (int) cap.get(CV_CAP_PROP_FOURCC);
  //int fourcc = CV_FOURCC('M','J','P','G'); // files too big
  //int fourcc = CV_FOURCC('H','2','6','4'); // too slow
  int fourcc = CV_FOURCC('F','M','P','4'); // not bad
  fps = (int) cap.get(CV_CAP_PROP_FPS);
  //fps = 15; // if this is different from camera, we will down sample
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
  grabber.end_grabbing();
  if(output_video.isOpened())
    output_video.release();
}

bool Camera::get_latest_frame()
{
  cv::Mat new_frame;
  if(grabber.get_latest_frame(new_frame)) {
    cv::flip(new_frame, latest_frame, -1); // todo: make separate function and accessor
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
  log_entry_exit w("record_thread_proc");
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
  left_camera.cam_number = 0;
  right_camera.cam_number = 1;
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
  try {
    log_entry_exit w("StereoCamera::record_thread_proc");
    left_camera.prepare_video_writer(left_recording_path);
    right_camera.prepare_video_writer(right_recording_path);

    double fps = left_camera.get_fps();
    auto t_start = std::chrono::high_resolution_clock::now();
    auto t_next_frame = t_start;
    std::chrono::microseconds us_per_frame((int) (1E6/fps) );

    // todo: change this to write when we get a frame ready event from camera

    while(this->record_on.load()) {
      t_next_frame += us_per_frame;
      std::this_thread::sleep_until(t_next_frame);
      if(right_camera.get_latest_frame()) {
        right_camera.write_latest_frame();
        ++frames_recorded;
        log_info("Wrote right frame");
      }
      if(left_camera.get_latest_frame()) {
        left_camera.write_latest_frame();
        ++frames_recorded;
        log_info("Wrote left frame");
      }
    }
    log_trace("closing stereo cameras");
    left_camera.release_video_writer();
    right_camera.release_video_writer();
  } catch (cv::Exception) {
    log_error("caught cv::Exception in StereoCamera::record_thread_proc");
  } catch (...) {
    log_error("unknown exception caught StereoCamera::record_thread_proc");
  }
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

