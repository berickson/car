#ifndef CAMERA_H
#define CAMERA_H

#include <thread>
#include <atomic>

#include <opencv2/core/core.hpp>
#include "opencv2/videoio.hpp"
#include <thread>
#include "frame_grabber.h"

class Camera
{
public:
  enum Mode {
    cap_320_by_240_by_30fps
  };
  int cam_number = 0;

  Mode mode = cap_320_by_240_by_30fps;

  Camera();
  void begin_capture_movie();
  void end_capture_movie();

  int get_frame_count_grabbed();
  int get_frame_count_saved();

private:
  int frame_count_saved = 0;
  cv::Mat latest_frame;

  std::atomic<bool> record_on; //this is lock free

  std::thread record_thread;
  void record_thread_proc();

  cv::VideoCapture cap;

  FrameGrabber grabber;

};

void test_camera();


#endif // CAMERA_H
