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
#include "image_utils.h"




StereoCamera::StereoCamera()
{
  left_camera.cam_number = 2;
  right_camera.cam_number = 0;

  cameras.push_back(&left_camera);
  cameras.push_back(&right_camera);

  cv::Mat M1,M2,D1,D2,R,T,R1,R2,P1,P2,Q;
  cv::Size size(640,480);

  // elp1 646 x 480
  M1 = (cv::Mat1d(3, 3) <<
        578.6422955382172, 0, 306.5516897586966,
         0, 578.6422955382172, 230.5388472155848,
         0, 0, 1);
  M2 = (cv::Mat1d(3, 3) << 578.6422955382172, 0, 316.3192203381367,
        0, 578.6422955382172, 238.075524732416,
        0, 0, 1);
  D1 = (cv::Mat1d(1,5) << -0.4261058146281724, 0.2310224765734376, 0, 0, -0.08302084026120671);
  D2 = (cv::Mat1d(1,5) << -0.4248453464459841, 0.2369092892971611, 0, 0, -0.08959892038971246);
  R1 = (cv::Mat1d(3,3) << 0.9999709614218707, -0.007079859138640332, 0.002819912693053336,
        0.00707164728009287, 0.9999707570613194, 0.00291149909112607,
        -0.00284044323396682, -0.002891473117406309, 0.9999917855989848);
  R2 = (cv::Mat1d(3,3) << 0.9998773805533686, -0.007080438591019467, 0.01396750683167339,
        0.00712093684852391, 0.9999705791373986, -0.002851862201288895,
        -0.01394690346038773, 0.002950974241603576, 0.9998983826544036);
  P1 = (cv::Mat1d(3,4) << 550.5636574538069, 0, 300.4732666015625, 0,
        0, 550.5636574538069, 232.1323795318604, 0,
        0, 0, 1, 0);

  P2 = (cv::Mat1d(3,4) << 550.5636574538069, 0, 300.4732666015625, -34.01531860221861,
        0, 550.5636574538069, 232.1323795318604, 0,
        0, 0, 1, 0);

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

  cv::Rect roi1, roi2;
  //cv::stereoRectify(M1, D1, M2, D2, size, R, T, R1, R2, P1, P2, Q, flags, alpha, size, &roi1, &roi2);

  cv::initUndistortRectifyMap(M1, D1, R1, P1, size, CV_16SC2, map11, map12);
  cv::initUndistortRectifyMap(M2, D2, R2, P1, size, CV_16SC2, map21, map22);
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

void StereoCamera::process_disparities(const cv::Mat L, const cv::Mat R)
{
  if(!L.empty() && !R.empty()) {
    int max_disparity = 64;
    int block_size = 25;
    cv::Mat im_disparity = cv::Mat( L.rows, L.cols, CV_16S );

    cv::Ptr<cv::StereoBM> matcher = cv::StereoBM::create(max_disparity, block_size);

    matcher->setUniquenessRatio(10);
    const cv::Mat L_gray = fast_bgr_to_gray(L);
    const cv::Mat R_gray = fast_bgr_to_gray(R);
    // search the three regions
    int w = L_gray.size[1];
    int h = L_gray.size[0];
    int roi_center = h/2-40;
    int box_height = 30;
    matcher->compute(L_gray, R_gray, im_disparity);

    double min_val; double max_val;
    cv::minMaxLoc(im_disparity, &min_val, &max_val);
    cv::Scalar label_color = cv::Scalar(max_val,0,0);
    int disparity_left = max_disparity + block_size/2;
    int disparity_right = w - block_size/2;
    int box_width = (disparity_right-disparity_left)/3;
    for(int i : {1,0,2}) {
      cv::Rect roi_rect = cv::Rect(disparity_left + i*box_width, roi_center-box_height/2,box_width,box_height);
      cv::rectangle(im_disparity, roi_rect, label_color);
      cv::Mat im_roi = cv::Mat(im_disparity,roi_rect);
      cv::Mat mask = im_roi>0;
      int dilation_size = 2;
      cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                                           cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                           cv::Point( dilation_size, dilation_size ) );
      cv::erode(mask,mask,element);
      double mean_disparity = cv::mean(im_roi,mask)[0];
      disparities[i] = mean_disparity;
    }
  }

}


void StereoCamera::end_recording()
{
  record_on.store(false);
  if(record_thread.joinable()){
    record_thread.join();
  }

}

string StereoCamera::get_clear_driving_direction() {
  stringstream s;
  s << "disparities: [" << disparities[0] << ", " << disparities[1] << ", " << disparities[2] << "]";
  log_info(s.str());

  if(disparities[1] <= disparities[2] && disparities[1] <= disparities[0]) {
    return "center";
  }
  if(disparities[0] <= disparities[1] && disparities[0] <= disparities[2]) {
    return "left";
  }
  if(disparities[2] <= disparities[0] && disparities[2] <= disparities[1]) {
    return "right";
  }
  return "unknown";
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
      if(right_camera.frame_is_ready() && left_camera.frame_is_ready()) {
        // since both are ready, we shouldn't have any problem reading both cameras
        if(!right_camera.get_latest_frame()) throw string("error reading right camera");
        if(!left_camera.get_latest_frame()) throw string("error reading left camera");

        // undistort both imags
        cv::remap(right_camera.latest_frame, right_camera.latest_frame, map21, map22, cv::INTER_LINEAR);
        cv::remap(left_camera.latest_frame, left_camera.latest_frame, map11, map12, cv::INTER_LINEAR);

        if(process_disparities_enabled) {
          process_disparities(left_camera.latest_frame, right_camera.latest_frame);
        }

        ++frames_processed;
      }
      if(frames_processed > 0) {
        left_camera.write_latest_frame();
        right_camera.write_latest_frame();
        ++frames_recorded;
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
    cout << "frames processed: " << camera.frames_processed << endl;
  }
  camera.end_recording();
}

