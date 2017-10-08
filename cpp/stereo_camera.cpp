#include "stereo_camera.h"
#include <iostream>
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




StereoCamera::StereoCamera()
{
  left_camera.cam_number = 1;
  right_camera.cam_number = 0;

  cameras.push_back(&left_camera);
  cameras.push_back(&right_camera);

  cv::Mat M1,M2,D1,D2,R,T,R1,R2,P1,P2,Q;
  cv::Size size(640,480);

  // elp1 646 x 480
  M1 = (cv::Mat1d(3, 3) <<  609.60113163, 0. , 305.41382098,    0., 609.60113163, 234.90158979, 0.f, 0., 1.);
  M2 = (cv::Mat1d(3, 3) << 596.42546952,0.,321.9541234,
                           0., 596.42546952,  238.85370978,
                           0.,            0.,            1.);
  D1 = (cv::Mat1d(1,5) << -4.68780696e-01, 2.87407784e-01, -1.81020633e-03, -2.21393570e-04, -1.19521770e-01);
  D2 = (cv::Mat1d(1,5) << -0.46036868, 0.3026241, -0.00086235, -0.00151008, -0.1524162);
  R1 = (cv::Mat1d(3,3) << 0.98798172, -0.02461123,  0.15259885,
                          0.02354489,  0.99968413,  0.00879127,
                         -0.15276701, -0.00509269,  0.98824911);
  R2 = (cv::Mat1d(3,3) << 0.98603072, -0.02269567, 0.16501008,
                          0.0238481,   0.99970306, -0.00500595,
                          -0.16484747, 0.0088712,   0.98627918);
  P1 = (cv::Mat1d(3,4) << 579.09150482,    0.,          193.77229309,    0.,
                            0.,          579.09150482,  232.33071899,    0.,
                            0.,           0.,           1.,           0.);

  P2 = (cv::Mat1d(3,4) << 579.09150482,    0.,          193.77229309,  -35.47708751,
                          0.,          579.09150482,  232.33071899,    0.,
                          0.,            0.,            1.,            0.);

 /*

  Q [[   1.            0.            0.         -193.77229309]
   [   0.            1.            0.         -232.33071899]
   [   0.            0.            0.          579.09150482]
   [   0.            0.           16.32297197   -0.        ]]
  R [[  9.99925082e-01   4.12658886e-04  -1.22335572e-02]
   [ -2.40237247e-04   9.99900670e-01   1.40922747e-02]
   [  1.22381573e-02  -1.40882799e-02   9.99825859e-01]] angle 1.06945169127 degrees
  T [[-0.06040755]
   [ 0.00139041]
   [-0.01010907]]
  roi1 (0, 0, 640, 480)
  roi2 (0, 0, 640, 480)
  */

  int flags = cv::CALIB_ZERO_DISPARITY;
  double alpha = 0;
  cv::Rect roi1, roi2;
  //cv::stereoRectify(M1, D1, M2, D2, size, R, T, R1, R2, P1, P2, Q, flags, alpha, size, &roi1, &roi2);

  cv::initUndistortRectifyMap(M1, D1, R1, P1, size, CV_16SC2, map11, map12);
  cv::initUndistortRectifyMap(M2, D2, R2, P2, size, CV_16SC2, map21, map22);
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

    double fps = 5;//left_camera.get_fps();
    auto t_start = std::chrono::high_resolution_clock::now();
    auto t_next_frame = t_start;
    std::chrono::microseconds us_per_frame((int) (1E6/fps) );

    // todo: change this to write when we get a frame ready event from camera

    while(this->record_on.load()) {
      t_next_frame += us_per_frame;
      std::this_thread::sleep_until(t_next_frame);
      if(right_camera.get_latest_frame()) {
        cv::remap(right_camera.latest_frame, right_camera.latest_frame, map21, map22,CV_INTER_LINEAR);
        right_camera.write_latest_frame();
        ++frames_recorded;
        log_info("Wrote right frame");
      }
      if(left_camera.get_latest_frame()) {
        cv::remap(left_camera.latest_frame, left_camera.latest_frame, map11, map12,CV_INTER_LINEAR);
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

