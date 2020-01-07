#include "camera.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
#include "geometry.h"
#include "logger.h"

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/videoio.hpp>
#include <thread>
#include <unistd.h> // usleep

#include <iostream>
#include <fstream>
#include <chrono>

#include "json.hpp"

#include "string_utils.h"


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
  if(!cap.isOpened()) throw string("couldn't open camera"+to_string(cam_number));
  
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
  
  cap.set(cv::CAP_PROP_FRAME_WIDTH,640);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT,480);
  cap.set(cv::CAP_PROP_FPS,15);
  cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M','J','P','G'));

  grabber.begin_grabbing(&cap, to_string(cam_number));
  warmed_up = true;
  log_info((string)"warmed up"+to_string(cam_number));
  
}

void Camera::prepare_video_writer(string path)
{
  cv::Size frame_size = cv::Size((int) cap.get(cv::CAP_PROP_FRAME_WIDTH),    // Acquire input size
                (int) cap.get(cv::CAP_PROP_FRAME_HEIGHT));
  //int fourcc = (int) cap.get(CV_CAP_PROP_FOURCC);
  //int fourcc = CV_FOURCC('M','J','P','G'); // files too big
  //int fourcc = CV_FOURCC('H','2','6','4'); // too slow
  int fourcc = cv::VideoWriter::fourcc('F','M','P','4'); // not bad
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

  output_video.open(recording_path , fourcc, (double)fps, frame_size, is_color);
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

  // write to a JPEG
  std::vector<uchar> buff;//buffer for coding
  std::vector<int> param(2);
  param[0] = cv::IMWRITE_JPEG_QUALITY;
  param[1] = 80;//default(95) 0-100
  cv::imencode(".jpg", latest_frame, buff, param);
  
  ofstream jpeg_file;
  jpeg_file.open (recording_path+to_fixed_width_string(frame_count_saved,5,'0')+".jpeg");
  jpeg_file.write((char*)&buff[0], buff.size());
  jpeg_file.close();
  
  output_video.write(latest_frame);
  ++frame_count_saved;
}

void Camera::undistort(cv::Mat frame) {
  cv::Mat undistorted;
  cv::undistort(frame, undistorted, camera_matrix, dist_coefs);
  undistorted.copyTo(frame);
}

bool Camera::frame_is_ready()
{
  return grabber.ready_frame_count() > 0;
}

void Camera::load_calibration_from_json(string camera_name, string json_path) {
  std::ifstream json_file(json_path);
  nlohmann::json calibration_json = nlohmann::json::parse(json_file);

  auto j = calibration_json.find(camera_name.c_str());
  if(j==calibration_json.end())  {
    throw string("could not find camera in json calibration");
  }
  auto m=j->at("camera_matrix");
  camera_matrix = (cv::Mat1d(3, 3) <<  m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

  auto d = j->at("dist_coefs");
  dist_coefs = (cv::Mat1d(1, 5) << d[0][0], d[0][1], d[0][2], d[0][3], d[0][4]);
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


