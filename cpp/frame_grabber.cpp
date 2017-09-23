#include "frame_grabber.h"
#include <map>
#include <unistd.h> // usleep
#include "logger.h"

using namespace std;


FrameGrabber::FrameGrabber(){
  grab_on.store(false);

}

FrameGrabber::~FrameGrabber() {
  end_grabbing();
}

void FrameGrabber::begin_grabbing(cv::VideoCapture * _cap, string _name = "unnamed")
{
  name = _name;
  cap = _cap;
  grab_thread = thread(&FrameGrabber::grab_thread_proc, this);
  grab_on.store(true);
}

void FrameGrabber::end_grabbing() {
  if(grab_on.load()==true) {
    grab_on.store(false);    //stop the grab loop
    grab_thread.join();      //wait for the grab loop

    while (!buffer.empty())   //flush buffer
    {
      buffer.pop();
    }
  }
}

bool FrameGrabber::get_one_frame(cv::Mat & frame)
{
  lock_guard<mutex> lock(grabber_mutex);
  if (buffer.size() == 0)
    return false;

  frame = buffer.front();   //get the oldest grabbed frame (queue=FIFO)
  buffer.pop();             //release the queue item
  return true;

}

bool FrameGrabber::get_latest_frame(cv::Mat & frame) {
  {
    lock_guard<mutex> lock(grabber_mutex);
    while(buffer.size()>1) {
      buffer.pop();
    }
  }
  return get_one_frame(frame);
}

int FrameGrabber::get_frame_count_grabbed() {
  return frames_grabbed;
}


void FrameGrabber::grab_thread_proc()
{
  try {
    while (grab_on.load() == true) //this is lock free
    {
      cv::Mat frame;
      //grab will wait for frame
      if(!cap->read(frame)){
        usleep(1000);
        continue;
      }
      {
        // only lock after frame is grabbed
        lock_guard<mutex> lock(grabber_mutex);
        ++frames_grabbed;
        log_info((string)"grabbed " + to_string(frames_grabbed) + " frames from  " + name + " buffer size: " + to_string(buffer.size()));
        buffer.push(frame);
      }
    }
  } catch (cv::Exception &e) {
    log_error("caught cv::Exception in FrameGrabber::grab_thread_proc");
    log_error(e.what());
  } catch (...) {
    log_error("unknown exception caught FrameGrabber::grab_thread_proc");
  }
}
