#ifndef CAMERA_H
#define CAMERA_H

#include <thread>
#include <atomic>

#include <opencv2/core/core.hpp>
#include "opencv2/videoio.hpp"
#include <thread>

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
  bool quit;
  std::atomic<int> frame_count;

private:
  cv::VideoCapture cap;
  void grab_thread_proc();

  std::thread grab_thread;
  std::string grab_thread_error_string;

};

void test_camera();


#endif // CAMERA_H
