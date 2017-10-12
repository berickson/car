#include "stereo_window.h"
#include "ui_stereo_window.h"
#include <sstream>
#include <opencv2/calib3d.hpp>
#include <qcheckbox.h>
#include <vector>


using namespace cv;

void show_image(cv::Mat & mat, QLabel & label ) {
  cv::cvtColor(mat, mat, CV_BGR2RGB);
  QImage imdisplay ((uchar*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
  label.setFixedSize(imdisplay.size());

  QPixmap pixmap = QPixmap::fromImage(imdisplay);
  label.setPixmap(QPixmap::fromImage(imdisplay));
}

StereoWindow::StereoWindow(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::StereoWIndow)
{
  ui->setupUi(this);
  left_camera.name = "elp1_left_640_480";
  std::string base_name = "/home/brian/car/tracks/front-sidewalk/routes/B/runs/2/video_";
  //std::string base_name = "/home/brian/car/debug/";
  left_camera.cap.open(base_name + "left.avi");
  right_camera.name = "elp1_left_640_480";
  right_camera.cap.open(base_name + "right.avi");
  left_camera.bound_label = ui->left_image;
  left_camera.parent = this;
  right_camera.bound_label = ui->right_image;
  right_camera.parent = this;

  int frame_count = (int) left_camera.cap.get(cv::CAP_PROP_FRAME_COUNT);
  ui->frame_count->setText( QString::number(frame_count));
  ui->frame_slider->setMaximum(frame_count);
  ui->frame_slider->setValue(50);
}


StereoWindow::~StereoWindow()
{
  delete ui;
}

void StereoWindow::on_frame_slider_valueChanged(int value)
{
  show_frame(value);
}

void StereoWindow::CameraUnit::detect_features(cv::Rect2d valid_rect) {
  auto detector = cv::xfeatures2d::SURF::create();
  cvtColor( frame, gray, CV_BGR2GRAY);
  cv::blur( gray, gray, cv::Size(5,5));

  std::vector<cv::KeyPoint> all_keypoints;
  detector->detect(gray,all_keypoints);
  keypoints.clear();
  // remove keypoints outside valid rect
  for(cv::KeyPoint & k : all_keypoints) {
    auto pt = k.pt;
    if(pt.x < valid_rect.x
       || pt.x > valid_rect.x + valid_rect.width
       || pt.y < valid_rect.y
       || pt.y > valid_rect.y + valid_rect.height) {
      continue;
    }
    keypoints.push_back(k);
  }
  detector->compute(gray,keypoints, descriptors);


  if(parent->ui->show_features_checkbox->checkState() == Qt::CheckState::Checked) {
    drawKeypoints(
          frame,
          keypoints,
          frame,
          cv::Scalar(0,255,0),
          cv::DrawMatchesFlags::DRAW_OVER_OUTIMG );//| cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

  }
}

void StereoWindow::CameraUnit::undistort_frame() {
  camera.load_calibration_from_json(name ,"/home/brian/car/camera_calibrations.json");
  camera.undistort(frame);

}

// Calculates rotation matrix to euler angles
// The result is the same as MATLAB except the order
// of the euler angles ( x and z are swapped ).
cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R)
{
  using namespace cv;

    //assert(isRotationMatrix(R));

    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );

    bool singular = sy < 1e-6; // If

    float x, y, z;
    if (!singular)
    {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3f(x, y, z);
}

void StereoWindow::show_frame(int number)
{
  ui->frame_slider->setValue(number);

  for(CameraUnit * camera : cameras) {
    if(camera->grab_frame(number) == false)
      return;
    if(ui->undistort_checkbox->isChecked()) {
      camera->undistort_frame();
    }
    cv::Rect2i valid_rect;
    valid_rect.x=0;
    valid_rect.y=0;
    valid_rect.width=640;
    valid_rect.height=320;
    camera->detect_features(valid_rect);
  }

  cv::Mat L = left_camera.frame;
  cv::Mat R = right_camera.frame;
  if(ui->depth_map_checkbox->isChecked() && !L.empty() && !R.empty()) {
    int max_disparity = 320;
    int block_size = 5;
    Mat im_disparity = Mat( L.rows, L.cols, CV_16S );
    cv::Ptr<StereoSGBM> matcher = cv::StereoSGBM::create(0, (10)*16, 11, 100, 1000, 32, 0, 15, 1000, 16, cv::StereoSGBM::MODE_HH);
    matcher->compute(L, R, im_disparity);
    double min_val; double max_val;
    cv::minMaxLoc(im_disparity, &min_val, &max_val);
    min_val /= 16;
    max_val /= 16;
    // turn the disparity into a distance at center


    // compute the real-world distance [mm]

    //Mat distance = Mat(L.rows, L.cols, CV_32F);
    //b = 0.062;
    //f=592;
    //distance = b*f/d
    cv::Mat im_display = im_disparity > (min_val*16+50);

    std::stringstream caption;
    caption << "min_val:" << min_val;
    caption << " max_val:" << max_val;
    cv::normalize(im_disparity, im_display, 0, 255*255,NORM_MINMAX);
    cv::putText(im_display, caption.str(), cv::Point(50,50),cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255*255));

    cv::imshow("disparity", im_display);
  }

  if(ui->match_features_checkbox->isChecked()) {
    cv::BFMatcher matcher;
    std::vector<cv::DMatch> matches;
    matcher.match(left_camera.descriptors, right_camera.descriptors, matches);


    // sort the matches by distance
    sort(matches.begin(), matches.end(),
        [](const cv::DMatch & a, const cv::DMatch& b) -> bool
    {
        return a.distance < b.distance;
    });


    {
      std::vector<cv::Point2f> left_points;
      std::vector<cv::Point2f> right_points;
      for(unsigned int i = 0; i < matches.size()/2; i++ ) {
        cv::DMatch match = matches.at(i);
        left_points.push_back(left_camera.keypoints[match.queryIdx].pt);
        right_points.push_back(right_camera.keypoints[match.trainIdx].pt);
      }
      // todo: find more parameters
      cv::Size image_size(640,480);
      double aperture_width = 1.0;
      double aperture_height = 1.0;
      double fov_x;
      double fov_y;
      double focal_length;
      cv::Point2d principle_point;
      double aspect_ratio;

      cv::calibrationMatrixValues(
            left_camera.camera.camera_matrix,
            image_size,
            aperture_width,
            aperture_height,
            fov_x,
            fov_y,
            focal_length,
            principle_point,
            aspect_ratio);

      cv::Mat camera_matrix = left_camera.camera.camera_matrix;
      cv::Mat E, R, t, mask;
      double probability = 0.999;
      double threshold = 1.0; // Maximum allowed reprojection error to treat a point pair as an inlier
      E = cv::findEssentialMat(
            left_points,
            right_points,
            camera_matrix,
            //focal_length,
            //principle_point,
            cv::RANSAC,
            probability,
            threshold,
            mask);

      int pose_result = cv::recoverPose(E, left_points, right_points, camera_matrix, R, t, mask);
      std::stringstream ss;
      ss << "R:" << rotationMatrixToEulerAngles(R) * 180/M_PI << std::endl << " t:" << t << std::endl << "pose_result:" << pose_result;
      ui->log_output->setText( QString::fromStdString(ss.str()));

      for(unsigned int i = 0; i < left_points.size(); i++ ) {
        int is_masked = mask.at<char>(i,0);
        if(is_masked == 0) continue;
        cv::Scalar color = cv::Scalar(rand()%255,rand()%255, rand()%255);
        cv::circle(left_camera.frame, left_points[i], 3, color);
        cv::circle(right_camera.frame, right_points[i], 3, color);

        //left_camera.show_match(match.queryIdx, color);
        //right_camera.show_match(match.trainIdx, color);
      }


    }
  }

  left_camera.show();
  right_camera.show();


}

int StereoWindow::get_frame_number()
{
  return ui->frame_slider->value();
}

void StereoWindow::on_show_features_checkbox_toggled(bool )
{
    show_frame(get_frame_number());
}

bool StereoWindow::CameraUnit::grab_frame(int frame_number) {
  cap.set(cv::CAP_PROP_POS_FRAMES, frame_number);
  return cap.read(frame);
}


void StereoWindow::CameraUnit::show() {
  show_image(frame, *bound_label);
}


void StereoWindow::CameraUnit::show_match(int feature_index, cv::Scalar color) {
  cv::KeyPoint keypoint = keypoints.at(feature_index);
  cv::circle(frame, keypoint.pt, 3, color);

}

void StereoWindow::on_match_features_checkbox_toggled(bool ) {
  show_frame(get_frame_number());
}
