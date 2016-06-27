#include "camera.h"
#include <string>
#include <vector>
#include <algorithm>
#include <array>
#include "geometry.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"


using namespace std;
using namespace cv;


Camera::Camera()
{
}


class Tracker {
public:
  bool draw_features = false;
  Mat gray_image;
  //cv::Ptr<Feature2D> sift = xfeatures2d::SIFT::create();
  cv::Ptr<Feature2D> detector = xfeatures2d::SURF::create();
  vector<KeyPoint> keypoints;
  Mat descriptors;
  double low_distance,high_distance;


  void process_image(cv::Mat image, bool annotate = true) {
    cvtColor( image, gray_image, CV_BGR2GRAY );
    blur(gray_image,gray_image,Size(5,5));

    vector<KeyPoint> previous_keypoints(keypoints);

    detector->detect(gray_image,keypoints);
    if(annotate && false){
      drawKeypoints(image, keypoints, image, cv::Scalar(0,255,0),  DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
    }
    Mat previous_descriptors = descriptors.clone();
    detector->compute(gray_image, keypoints, descriptors );
    if(previous_keypoints.size()>0) {
      BFMatcher matcher;
      vector<DMatch> matches;
      matcher.match(descriptors, previous_descriptors, matches);

      vector<float> distances;
      for(auto match : matches) {
        auto p1 = previous_keypoints[match.trainIdx].pt;
        auto p2 = keypoints[match.queryIdx].pt;
        double d = ::distance(p1.x,p1.y,p2.x,p2.y);

        distances.push_back(d);
      }
      // require at least 10 matches to continue
      if(distances.size() < 10){
        return;
      }
      sort(distances.begin(),distances.end());
      low_distance = distances[(int)(.1*distances.size())]; // 10th percentile, probably buggy
      high_distance = distances[(int)(.3*distances.size())]; // 30th percentile, probably buggy


      for(DMatch& match: matches) {
        // skip out of range distances
        auto p1 = previous_keypoints[match.trainIdx].pt;
        auto p2 = keypoints[match.queryIdx].pt;
        double d = ::distance(p1.x,p1.y,p2.x,p2.y);

        if(d < low_distance || d > high_distance)
          continue;
        cv::line(
              image,
              previous_keypoints[match.trainIdx].pt,
              keypoints[match.queryIdx].pt,
              Scalar(0,255,0),5 );
      }
    }
  }

};




void Camera::capture_movie()
{
  int frame_count = 0;

  cv::VideoCapture cap;



  if(!cap.open(0)) throw (string) "couldn't open camera";
  cv::Mat frame;

  Tracker tracker;

  for(;;) {

    cap >> frame;
    tracker.process_image(frame);
    ++frame_count;
    const Scalar white = Scalar(255,255,255);
    putText(frame, (string) to_string(frame_count), cv::Point(50,50), FONT_HERSHEY_SIMPLEX ,1, white);
    putText(frame, (string) to_string(tracker.low_distance), cv::Point(50,100), FONT_HERSHEY_SIMPLEX ,1, white);
    putText(frame, (string) to_string(tracker.high_distance), cv::Point(50,150), FONT_HERSHEY_SIMPLEX ,1, white);

    cv::imshow("window 1", frame);
    if(cv::waitKey(1)==27) break;
  }
}



void test_camera() {

  Camera camera;
  camera.capture_movie();


}

