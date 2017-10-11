// Example 19-3. Stereo calibration, rectification, and correspondence
#pragma warning(disable : 4996)
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <glob.h>
#include <vector>
#include <string>
#include <algorithm>

inline std::vector<std::string> glob(const std::string& pat){
    using namespace std;
    glob_t glob_result;
    glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
    vector<string> ret;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        ret.push_back(string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    sort(ret.begin(), ret.end());
    return ret;
}

using namespace std;



static void StereoCalib(vector<string> paths, int nx, int ny,
                        bool useUncalibrated) {
  bool displayCorners = false;
  bool showUndistorted = true;
  bool isVerticalStereo = false; // horiz or vert cams
  const int maxScale = 1;
  // const float squareSize = 0.02349f; // original
  const float squareSize = 0.02652f; // "big"

  // actual square size
  int i, j, lr;
  int N = nx * ny;
  cv::Size board_sz = cv::Size(nx, ny);
  vector<string> imageNames[2];
  vector<cv::Point3f> boardModel;
  vector<vector<cv::Point3f> > objectPoints;
  vector<vector<cv::Point2f> > points[2];
  vector<cv::Point2f> corners[2];
  bool found[2] = {false, false};
  cv::Size imageSize;

  // READ IN THE LIST OF CIRCLE GRIDS:
  //
  for (i = 0; i < ny; i++)
    for (j = 0; j < nx; j++)
      boardModel.push_back(
          cv::Point3f((float)(i * squareSize), (float)(j * squareSize), 0.f));
  for (int i=0;i<paths.size();++i) {
    string buf = paths[i];
    lr = i % 2;
    if (lr == 0)
      found[0] = found[1] = false;
    cv::Mat img = cv::imread(buf, 0);
    if (img.empty())
      break;
    imageSize = img.size();
    imageNames[lr].push_back(buf);


    // If we did not find board on the left image,
    // it does not make sense to find it on the right.
    //
    if (lr == 1 && !found[0]) {
      continue;
    }

    // Find circle grids and centers therein:
    for (int s = 1; s <= maxScale; s++) {
      cv::Mat timg = img;
      if (s > 1)
        resize(img, timg, cv::Size(), s, s, cv::INTER_CUBIC);
      // Just as example, this would be the call if you had circle calibration
      // boards ...
      //      found[lr] = cv::findCirclesGrid(timg, cv::Size(nx, ny),
      //      corners[lr],
      //                                      cv::CALIB_CB_ASYMMETRIC_GRID |
      //                                          cv::CALIB_CB_CLUSTERING);
      //...but we have chessboards in our images
      found[lr] = cv::findChessboardCorners(timg, board_sz, corners[lr]);
      if (!found[lr]) {
        cout << "couldn't find left chessboard for " << paths[i] << endl;
      }

      if (found[lr] || s == maxScale) {
        cv::Mat mcorners(corners[lr]);
        mcorners *= (1. / s);
      }
      if (found[lr])
        break;
    }
    if (displayCorners) {
      cout << buf << endl;
      cv::Mat cimg;
      cv::cvtColor(img, cimg, cv::COLOR_GRAY2BGR);

      // draw chessboard corners works for circle grids too
      cv::drawChessboardCorners(cimg, cv::Size(nx, ny), corners[lr], found[lr]);
      cv::imshow("Corners", cimg);
      if ((cv::waitKey(0) & 255) == 27) // Allow ESC to quit
        exit(-1);
    } else
      cout << '.';
    if (lr == 1 && found[0] && found[1]) {
      objectPoints.push_back(boardModel);
      points[0].push_back(corners[0]);
      points[1].push_back(corners[1]);
    }
  }

  // CALIBRATE THE STEREO CAMERAS
  cv::Mat M1 = cv::Mat::eye(3, 3, CV_64F);
  cv::Mat M2 = cv::Mat::eye(3, 3, CV_64F);
  cv::Mat D1, D2, R, T, E, F;
  cout << "\nRunning stereo calibration ..." << endl;
  cv::stereoCalibrate(
      objectPoints, points[0], points[1], M1, D1, M2, D2, imageSize, R, T, E, F,
      cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_ZERO_TANGENT_DIST |
          cv::CALIB_SAME_FOCAL_LENGTH,
      cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 100,
                       1e-5));
  cout << "Done! Press any key to step through images, ESC to exit" << endl << endl;

  // CALIBRATION QUALITY CHECK
  // because the output fundamental matrix implicitly
  // includes all the output information,
  // we can check the quality of calibration using the
  // epipolar geometry constraint: m2^t*F*m1=0
  vector<cv::Point3f> lines[2];
  double avgErr = 0;
  int nframes = (int)objectPoints.size();
  for (i = 0; i < nframes; i++) {
    vector<cv::Point2f> &pt0 = points[0][i];
    vector<cv::Point2f> &pt1 = points[1][i];
    cv::undistortPoints(pt0, pt0, M1, D1, cv::Mat(), M1);
    cv::undistortPoints(pt1, pt1, M2, D2, cv::Mat(), M2);
    cv::computeCorrespondEpilines(pt0, 1, F, lines[0]);
    cv::computeCorrespondEpilines(pt1, 2, F, lines[1]);

    for (j = 0; j < N; j++) {
      double err = fabs(pt0[j].x * lines[1][j].x + pt0[j].y * lines[1][j].y +
                        lines[1][j].z) +
                   fabs(pt1[j].x * lines[0][j].x + pt1[j].y * lines[0][j].y +
                        lines[0][j].z);
      avgErr += err;
    }
  }
  cout << "avg err = " << avgErr / (nframes * N) << endl;

  // COMPUTE AND DISPLAY RECTIFICATION
  //
  if (showUndistorted) {
    cv::Mat R1, R2, P1, P2, map11, map12, map21, map22;

    // IF BY CALIBRATED (BOUGUET'S METHOD)
    //
    if (!useUncalibrated) {
      cout << "calibrating with Boujet's method" << endl;
      double alpha = 0;
      cv::Rect roi1, roi2;
      stereoRectify(M1, D1, M2, D2, imageSize, R, T, R1, R2, P1, P2,
                    cv::noArray(), CV_CALIB_ZERO_DISPARITY, alpha, imageSize, &roi1, &roi2);
      isVerticalStereo = fabs(P2.at<double>(1, 3)) > fabs(P2.at<double>(0, 3));
      // Precompute maps for cvRemap()
      initUndistortRectifyMap(M1, D1, R1, P1, imageSize, CV_16SC2, map11,
                              map12);
      initUndistortRectifyMap(M2, D2, R2, P2, imageSize, CV_16SC2, map21,
                              map22);
      cout << "roi1: " << roi1 << endl;
      cout << "roi2: " << roi2 << endl;
    }

    // OR ELSE HARTLEY'S METHOD
    //
    else {
      cout << "calibrating with Hartley's method (uncalibrated)" << endl;

      // use intrinsic parameters of each camera, but
      // compute the rectification transformation directly
      // from the fundamental matrix
      vector<cv::Point2f> allpoints[2];
      for (i = 0; i < nframes; i++) {
        copy(points[0][i].begin(), points[0][i].end(),
             back_inserter(allpoints[0]));
        copy(points[1][i].begin(), points[1][i].end(),
             back_inserter(allpoints[1]));
      }
      cv::Mat F = findFundamentalMat(allpoints[0], allpoints[1], cv::FM_8POINT);
      cv::Mat H1, H2;
      cv::stereoRectifyUncalibrated(allpoints[0], allpoints[1], F, imageSize,
                                    H1, H2, 3);
      R1 = M1.inv() * H1 * M1;
      R2 = M2.inv() * H2 * M2;

      // Precompute map for cvRemap()
      //
      cv::initUndistortRectifyMap(M1, D1, R1, P1, imageSize, CV_16SC2, map11,
                                  map12);
      cv::initUndistortRectifyMap(M2, D2, R2, P2, imageSize, CV_16SC2, map21,
                                  map22);
    }
    cout << "M1 = \n" << format(M1, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "D1 = \n" << format(D1, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "R1 = \n" << format(R1, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "P1 = \n" << format(P1, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "M2 = \n" << format(M2, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "D2 = \n" << format(D2, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "R2 = \n" << format(R2, cv::Formatter::FMT_C) << ";" << endl << endl;
    cout << "P2 = \n" << format(P2, cv::Formatter::FMT_C) << ";" << endl << endl;


    // RECTIFY THE IMAGES AND FIND DISPARITY MAPS
    //
    cv::Mat pair;
    if (!isVerticalStereo)
      pair.create(imageSize.height, imageSize.width * 2, CV_8UC3);
    else
      pair.create(imageSize.height * 2, imageSize.width, CV_8UC3);

    // Setup for finding stereo corrrespondences
    //
    cv::Ptr<cv::StereoSGBM> stereo = cv::StereoSGBM::create(
        -64, 128, 11, 100, 1000, 32, 0, 15, 1000, 16, cv::StereoSGBM::MODE_HH);

    for (i = 0; i < nframes; i++) {
      cv::Mat img1 = cv::imread(imageNames[0][i].c_str(), 0);
      cv::Mat img2 = cv::imread(imageNames[1][i].c_str(), 0);
      cv::Mat img1r, img2r, disp, vdisp;
      if (img1.empty() || img2.empty())
        continue;
      cv::remap(img1, img1r, map11, map12, cv::INTER_LINEAR);
      cv::remap(img2, img2r, map21, map22, cv::INTER_LINEAR);
      if (!isVerticalStereo || !useUncalibrated) {

        // When the stereo camera is oriented vertically,
        // Hartley method does not transpose the
        // image, so the epipolar lines in the rectified
        // images are vertical. Stereo correspondence
        // function does not support such a case.
        stereo->compute(img1r, img2r, disp);
        cv::normalize(disp, vdisp, 0, 256, cv::NORM_MINMAX, CV_8U);
        cv::imshow("disparity", vdisp);
      }
      if (!isVerticalStereo) {
        cv::Mat part = pair.colRange(0, imageSize.width);
        cvtColor(img1r, part, cv::COLOR_GRAY2BGR);
        part = pair.colRange(imageSize.width, imageSize.width * 2);
        cvtColor(img2r, part, cv::COLOR_GRAY2BGR);
        for (j = 0; j < imageSize.height; j += 16)
          cv::line(pair, cv::Point(0, j), cv::Point(imageSize.width * 2, j),
                   cv::Scalar(0, 255, 0));
      } else {
        cv::Mat part = pair.rowRange(0, imageSize.height);
        cv::cvtColor(img1r, part, cv::COLOR_GRAY2BGR);
        part = pair.rowRange(imageSize.height, imageSize.height * 2);
        cv::cvtColor(img2r, part, cv::COLOR_GRAY2BGR);
        for (j = 0; j < imageSize.width; j += imageSize.height / 16)
          line(pair, cv::Point(j, 0), cv::Point(j, imageSize.height * 2),
               cv::Scalar(0, 255, 0));
      }
      cv::imshow("rectified", pair);
      if ((cv::waitKey() & 255) == 27)
        break;
    }
  }
}

//
//Default Call (with parameters: board_w = 9, board_h = 6, list =
//  ../stereoData_19-03_list.txt):
//./example_19-03
//
//Manual call:
//./example_19-03 [<board_w> <board_h> <path/list_of_stereo_pairs>]
//
// Press any key to step through results, ESC to exit
//




int main(int argc, char **argv) {
  int board_w = 9, board_h = 6;
  string folder = "/home/brian/race-vision/input/chessboard_images/stereo/elp1bigchessboard/640_480/";
  vector<string> paths;
  vector<string> left = glob(folder+"*left.png");
  vector<string> right= glob(folder+"*right.png");
  for(int i = 0; i < left.size(); ++i) {
    paths.push_back(left[i]);
    paths.push_back(right[i]);
  }

  StereoCalib(paths, board_w, board_h, false);
  return 0;
}
