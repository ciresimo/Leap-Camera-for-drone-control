#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "Leap.h"

#ifndef SAMPLELISTENER_H
#define SAMPLELISTENER_H

class SampleListener : public Leap::Listener {
  public:

    bool _stop_program = false;
    int _frames_to_collect ;
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);    
    virtual void onFrame(const Leap::Controller&);    
    void setRecordStatus(bool);
    void setOutputFile(const std::string);
  

  private:
    bool _record = false;
    int _frames;
    std::string _output_file;
    cv::Mat leapImageToCvMat(const Leap::Image&);
    float* handData(const Leap::Hand& , const cv::Mat&);
};

#endif SAMPLELISTENER_H