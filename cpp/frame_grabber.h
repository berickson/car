#ifndef FRAME_GRABBER_H
#define FRAME_GRABBER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/videoio.hpp"


#include <thread>
#include <mutex>
#include <queue>
#include <atomic>
#include <string>
#include "work_queue.h"

// based on http://answers.opencv.org/question/74255/time-delay-in-videocapture-opencv-due-to-capture-buffer/



class FrameGrabber {
public:
  FrameGrabber();
  ~FrameGrabber();

  void begin_grabbing(cv::VideoCapture * cap, std::string name);
  void end_grabbing();
  bool get_one_frame(cv::Mat & frame);
  bool get_latest_frame(cv::Mat & frame);
  int  get_frame_count_grabbed();
  int ready_frame_count();

  cv::VideoCapture * cap;

  std::queue<cv::Mat> buffer;
  std::mutex grabber_mutex;
  std::atomic<bool> grab_on; //this is lock free
  int frames_grabbed = 0;
  unsigned int max_frames_to_buffer = 1;

  std::thread grab_thread;
  void grab_thread_proc();
  std::string name;
  ObservableTopic<cv::Mat> frames_topic;
};

#endif // FRAME_GRABBER_H
