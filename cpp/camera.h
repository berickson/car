#ifndef CAMERA_H
#define CAMERA_H

#include <thread>
#include <atomic>

#include <opencv2/core/core.hpp>
#include "opencv2/videoio.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <thread>
#include "frame_grabber.h"
#include <string>
#include <vector>

class Camera
{
public:
  enum Mode {
    cap_320_by_240_by_30fps
  };
  int cam_number = 0;
  double get_fps();

  Mode mode = cap_320_by_240_by_30fps;

  Camera();
  ~Camera();
  void set_recording_path(std::string path);
  void warm_up();
  void prepare_video_writer(std::string path);
  void begin_capture_movie();
  void end_capture_movie();
  void release_video_writer();
  bool get_latest_frame();
  void write_latest_frame();

  int get_frame_count_grabbed();
  int get_frame_count_saved();
  void load_calibration_from_json(std::string camera_name, std::string json_path);
  void undistort(cv::Mat frame);
  cv::Mat camera_matrix;
  cv::Mat dist_coefs;

  cv::Mat latest_frame;

private:
  int fps = 0;
  bool warmed_up = false;
  std::string recording_path;
  int frame_count_saved = 0;
  cv::VideoWriter output_video;

  std::atomic<bool> record_on; //this is lock free

  std::thread record_thread;
  void record_thread_proc();

  cv::VideoCapture cap;

  FrameGrabber grabber;

};


void test_camera();
void test_stereo_camera();


#endif // CAMERA_H
