#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "Leap.h"
#include "SampleListener.h"




using namespace Leap;
using namespace std;


const string fingerNames[] = {"Thumb", "Index", "Middle", "Ring", "Pinky"};
const string boneNames[] = {"Metacarpal", "Proximal", "Middle", "Distal"};
const string stateNames[] = {"STATE_INVALID", "STATE_START", "STATE_UPDATE", "STATE_END"};

void SampleListener::onInit(const Controller& controller) {
  cout << "Initialized" << endl;
}

void SampleListener::onConnect(const Controller& controller) {
  cout << "Connected" << endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  cout << "Disconnected" << endl;
}

void SampleListener::onExit(const Controller& controller) {
  cout << "Exited" << endl;
}

void SampleListener::onFrame(const Controller& controller) {
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();

  //Get thw two hands
  HandList hands = frame.hands();

  const Hand leftmost = hands.leftmost();
  const Hand rightmost = hands.rightmost();

  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = *hl;
    string handType = hand.isLeft() ? "Left hand" : "Right hand";
    cout << string(2, ' ') << handType << ", id: " << hand.id()
              << ", palm position: " << hand.palmPosition() << endl;
    float* orientation = orientationPRY(hand);
    cout <<"pitch: " << orientation[0] * RAD_TO_DEG << " degrees, "
         << "roll: " << orientation[1] * RAD_TO_DEG << " degrees, "
         << "yaw: " << orientation[2] * RAD_TO_DEG << " degrees" << endl;

  }
  
  onImage(controller,leftmost,rightmost);
 
  if (!frame.hands().isEmpty() ) {
    cout << endl;
  }

}

void SampleListener::onFocusGained(const Controller& controller) {
  cout << "Focus Gained" << endl;
}

void SampleListener::onFocusLost(const Controller& controller) {
  cout << "Focus Lost" << endl;
}

void SampleListener::onDeviceChange(const Controller& controller) {
  cout << "Device Changed" << endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    cout << "id: " << devices[i].toString() << endl;
    cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << endl;
  }
}

void SampleListener::onServiceConnect(const Controller& controller) {
  cout << "Service Connected" << endl;
}

void SampleListener::onServiceDisconnect(const Controller& controller) {
  cout << "Service Disconnected" << endl;
}

void SampleListener::onImage(const Controller& controller, const Hand& left_hand, const Hand& right_hand) {
    const Leap::ImageList images = controller.images();
    const Frame frame = controller.frame();

    if (!images.isEmpty()) {
      Leap::Image image = images[0];  // Assuming there is only one image    
      

        // Convert Leap image to OpenCV Mat
        cv::Mat cvImage = leapImageToCvMat(image);

        cv::putText(cvImage, "Hello OpenCV", cv::Point(50, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);

        //Draw the normal vectors of the hands
        const Vector normal_right = right_hand.palmNormal();

        // Display the image using OpenCV
        cv::imshow("Leap Motion Image", cvImage);
        cv::waitKey(1);

        //Resize the window
        cv::resizeWindow("Leap Motion Image",cv::Size(1000,1000));
    }
}

cv::Mat SampleListener::leapImageToCvMat(const Leap::Image& image) {
    int width = image.width();
    int height = image.height();
    const unsigned char* imageData = image.data();

    cv::Mat cvImage(height, width, CV_8UC1);
    memcpy(cvImage.data, imageData, width * height * sizeof(unsigned char));

    return cvImage;
}

float* SampleListener::orientationPRY(const Hand& hand){
  // Get the hand's normal vector and direction
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
    float* vec = new float[3];
    vec[0] = direction.pitch();
    vec[1] = normal.roll();
    vec[2] = direction.yaw();
  return vec;  

}

