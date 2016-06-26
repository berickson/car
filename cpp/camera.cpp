#include "camera.h"
#include <string>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "opencv2/videoio.hpp"


using namespace std;
using namespace cv;


Camera::Camera()
{
}



void add_features(cv::Mat image) {
  std::vector< cv::Point2f > corners;

  // maxCorners – The maximum number of corners to return. If there are more corners
  // than that will be found, the strongest of them will be returned
  int maxCorners = 50;

  // qualityLevel – Characterizes the minimal accepted quality of image corners;
  // the value of the parameter is multiplied by the by the best corner quality
  // measure (which is the min eigenvalue, see cornerMinEigenVal() ,
  // or the Harris function response, see cornerHarris() ).
  // The corners, which quality measure is less than the product, will be rejected.
  // For example, if the best corner has the quality measure = 1500,
  // and the qualityLevel=0.01 , then all the corners which quality measure is
  // less than 15 will be rejected.
  double qualityLevel = 0.01;

  // minDistance – The minimum possible Euclidean distance between the returned corners
  double minDistance = 3.;

  // mask – The optional region of interest. If the image is not empty (then it
  // needs to have the type CV_8UC1 and the same size as image ), it will specify
  // the region in which the corners are detected
  cv::Mat mask;

  // blockSize – Size of the averaging block for computing derivative covariation
  // matrix over each pixel neighborhood, see cornerEigenValsAndVecs()
  int blockSize = 30;

  // useHarrisDetector – Indicates, whether to use operator or cornerMinEigenVal()
  bool useHarrisDetector = false;

  // k – Free parameter of Harris detector
  double k = 0.04;

  Mat gray_image;
  blur(image,image,Size(5,5));
  cvtColor( image, gray_image, CV_BGR2GRAY );
///  blur(gray_image,gray_image,Size(50,50));

  // cv::goodFeaturesToTrack( gray_image, corners, maxCorners, qualityLevel, minDistance, mask, blockSize, useHarrisDetector, k );

  for( auto corner:corners) {
    cv::circle( image, corner, 10, cv::Scalar( 255.,0,0 ), 1 );
  }

  cv::Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();
  vector<KeyPoint> keypoints;
  f2d->detect(gray_image,keypoints);
  drawKeypoints(image, keypoints, image, cv::Scalar(0,255,0),  DrawMatchesFlags::DRAW_OVER_OUTIMG | DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
  //cv::xfeatures2d::SiftFeatureDetector detector;
  //std::vector<cv::KeyPoint> keypoints;
  //detector.detect(gray_image, keypoints);



}


void Camera::capture_movie()
{
  int frame_count = 0;

  cv::VideoCapture cap;



  if(!cap.open(0)) throw (string) "couldn't open camera";
  QtFont font = fontQt("Times");
  cv::Mat frame;

  for(;;) {

    cap >> frame;
    ++frame_count;
    // void putText(Mat& img, const string& text, Point org, int fontFace, double fontScale, Scalar color, int thickness=1, int lineType=8, bool bottomLeftOrigin=false )
    putText(frame, (string) to_string(frame_count), Point(50,50), FONT_HERSHEY_SIMPLEX ,1, Scalar(255));
    add_features(frame);

//    cv::addText(frame,"hello",cv::Point(0,0),font);

    cv::imshow("window 1", frame);
    if(cv::waitKey(1)==27) break;
  }
}



void test_camera() {

  Camera camera;
  camera.capture_movie();


}

