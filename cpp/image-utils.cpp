#include "image-utils.h"
#include <opencv2/core/core.hpp>

const cv::Mat fast_bgr_to_gray(const cv::Mat im) {
  cv::Mat channels[3];
  cv::split(im, channels);
  return channels[1]; // green
}
