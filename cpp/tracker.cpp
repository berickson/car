#include "tracker.h"

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <array>
#include "geometry.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"
#include "opencv2/calib3d.hpp"


using namespace std;

Tracker::Tracker() {
  detector = cv::xfeatures2d::SURF::create();
}

void Tracker::process_image(cv::Mat image, bool annotate) {
  cvtColor( image, gray_image, CV_BGR2GRAY );
  cv::blur(gray_image, gray_image, cv::Size(5,5));

  vector<cv::KeyPoint> previous_keypoints(keypoints);

  detector->detect(gray_image,keypoints);
  if(annotate && false){
    drawKeypoints(
          image,
          keypoints,
          image,
          cv::Scalar(0,255,0),
          cv::DrawMatchesFlags::DRAW_OVER_OUTIMG | cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  }
  cv::Mat previous_descriptors = descriptors.clone();
  detector->compute(gray_image, keypoints, descriptors );
  if(previous_keypoints.size()>0) {
    cv::BFMatcher matcher;
    vector<cv::DMatch> matches;
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
    low_distance = distances[(int)(0.*distances.size())]; // 10th percentile, probably buggy
    high_distance = distances[(int)(.7*distances.size())]; // 30th percentile, probably buggy


    vector<cv::Point> src;
    vector<cv::Point> dest;
    for(cv::DMatch& match: matches) {
      // skip out of range distances
      auto p1 = previous_keypoints[match.trainIdx].pt;
      auto p2 = keypoints[match.queryIdx].pt;
      double d = ::distance(p1.x,p1.y,p2.x,p2.y);

      if(d < low_distance || d > high_distance)
        continue;
      /*
      cv::line(
            image,
            previous_keypoints[match.trainIdx].pt,
          keypoints[match.queryIdx].pt,
          cv::Scalar(0,255,0),5 );
      */
      src.push_back(p1);
      dest.push_back(p2);

    }
    homography = cv::findHomography(src,dest,cv::RANSAC);

    if(! homography.empty()) {
      // make float points from integer points
      vector<cv::Point2f> from_points;
      vector<cv::Point2f> to_points;
      for(auto p:src) {
        cv::Point2f pf;
        pf.x = p.x;
        pf.y = p.y;
        from_points.push_back(pf);
        to_points.push_back(pf);
      }
      perspectiveTransform( from_points, to_points, homography);
      for(unsigned i = 0; i < from_points.size(); i++) {
        cv::line(
              image,
              from_points[i],
              to_points[i],
            cv::Scalar(0,255,255),5 );
      }
    }
  }
}


void test_tracker() {
  int frame_count = 0;

  cv::VideoCapture cap;
  cap.set(cv::CAP_PROP_FRAME_WIDTH,320);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT,240);

  if(!cap.open(0)) throw (string) "couldn't open camera";
  cv::Mat frame;

  Tracker tracker;

  for(;;) {

    cap >> frame;
    tracker.process_image(frame);
    ++frame_count;
    const cv::Scalar white = cv::Scalar(255,255,255);
    const cv::Scalar green = cv::Scalar(0,255,0);
    const cv::Scalar red = cv::Scalar(0,0,255);

    cv::putText(frame, (string) to_string(frame_count), cv::Point(50,50), cv::FONT_HERSHEY_SIMPLEX, 1, red, 3);
    //putText(frame, (string) to_string(tracker.low_distance), cv::Point(50,100), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);
    //putText(frame, (string) to_string(tracker.high_distance), cv::Point(50,150), cv::FONT_HERSHEY_SIMPLEX ,1, red, 3);

    cv::imshow("window 1", frame);
    if(cv::waitKey(1)==27) break;
  }

}
