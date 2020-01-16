#ifndef STEREO_CAMERA_H
#define STEREO_CAMERA_H

#include "camera.h"
#include <opencv2/calib3d.hpp>
#include <string>


class StereoCamera {
public:
  StereoCamera();

  void warm_up();
  void begin_recording(std::string left_recording_path, std::string right_recording_path);
  void process_disparities(const cv::Mat L, const cv::Mat R);
  void end_recording();
  std::string get_clear_driving_direction();

  double disparities[3] = {NAN, NAN, NAN};

  std::atomic<bool> record_on;

  std::thread record_thread;
  void record_thread_proc();
  int frames_recorded = 0;  // number of frames in avi, might include duplicates
  int frames_processed = 0; // number of unique frames processed
  bool process_disparities_enabled = false;

  // rectification maps
  cv::Mat map11,map12, map21, map22;
  std::string left_recording_path, right_recording_path;
  std::vector<Camera *> cameras;

  Camera left_camera;
  Camera right_camera;
  int left_cam_id = 1;
  int right_cam_id = 0;
};

#endif // STEREO_CAMERA_H
