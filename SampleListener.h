#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "Leap.h"

#ifndef SAMPLELISTENER_H
#define SAMPLELISTENER_H

class SampleListener : public Leap::Listener {
  public:
    virtual void onInit(const Leap::Controller&);
    virtual void onConnect(const Leap::Controller&);
    virtual void onDisconnect(const Leap::Controller&);
    virtual void onExit(const Leap::Controller&);
    virtual void onFrame(const Leap::Controller&);
    virtual void onFocusGained(const Leap::Controller&);
    virtual void onFocusLost(const Leap::Controller&);
    virtual void onDeviceChange(const Leap::Controller&);
    virtual void onServiceConnect(const Leap::Controller&);
    virtual void onServiceDisconnect(const Leap::Controller&);
    virtual void onImage(const Leap::Controller&);

  private:
    cv::Mat leapImageToCvMat(const Leap::Image&);
    float* orientationPRY(const Leap::Hand&);
};

#endif SAMPLELISTENER_H