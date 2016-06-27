#ifndef TRACKER_H
#define TRACKER_H

#include <opencv2/core/core.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/features2d.hpp>

class Tracker {
public:
  Tracker();

  bool draw_features = false;

  cv::Mat gray_image;

  cv::Ptr<cv::Feature2D> detector;
  std::vector<cv::KeyPoint> keypoints;
  cv::Mat descriptors;
  cv::Mat homography;
  double low_distance,high_distance;


  void process_image(cv::Mat image, bool annotate = true);
};

void test_tracker();

#endif // TRACKER_H
