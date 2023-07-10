#include <iostream>
#include <cstring>
#include <fstream>
#include <ctime>
#include <filesystem>
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
  _frames = 0;
  _frames_to_collect = 200;
}

void SampleListener::onConnect(const Controller& controller) {
  cout << "Connected" << endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);

  //Create the output file, or clean it if already existing
  ofstream outputFile;
  outputFile.open(_output_file);
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  cout << "Disconnected" << endl;
}

void SampleListener::onExit(const Controller& controller) {
  cout << "Exited" << endl;
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

void SampleListener::onFrame(const Controller& controller) {
  
  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  const Leap::ImageList images = controller.images();     

  if (!images.isEmpty()) {
    Leap::Image image = images[0];  // Assuming there is only one image    
    

    // Convert Leap image to OpenCV Mat
    cv::Mat cvImage = leapImageToCvMat(image);

    //Get thw two hands
    HandList hands = frame.hands();

    if (!frame.hands().isEmpty() ) {    
      
      const Hand leftmost = hands.leftmost();
      const Hand rightmost = hands.rightmost(); 

      
      // float* left_data = handData(leftmost,cvImage);
      float* right_data = handData(rightmost,cvImage);

      
    }
    
    cv::putText(cvImage, "Leap Motion Camera", cv::Point(50, 30), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(255, 0, 255), 2);



    //Start and stop collecting data
    int check = cv::waitKey(10);
    
    if(check == 49){

      setRecordStatus(true);
      _frames = 0;
      cout<<"Start recording"<<endl;
      
    }
    else if(check == 50 || _frames == _frames_to_collect){

      setRecordStatus(false);
      _frames = 0;
      cout<<"Stop recording"<<endl;

    }
    else if(check == 51){

      _stop_program = true;
      cout<<"Stopping the program"<<endl;

    }
    
    //When recording, count the number of frames. Stop recording at 50 frames
    if(_record){

      cout<<_frames++<<endl;
      cv::putText(cvImage, "Recording", cv::Point(525, 150),  cv::FONT_HERSHEY_SIMPLEX , 0.55, cv::Scalar(255, 0, 0), 1);
    }

    // Display the image using OpenCV
    cv::imshow("Leap Motion Image", cvImage);
    
    //Resize the window
    cv::resizeWindow("Leap Motion Image",cv::Size(1000,1000));

  }

}


cv::Mat SampleListener::leapImageToCvMat(const Leap::Image& image ) {
    int width = image.width();
    int height = image.height();
    const unsigned char* imageData = image.data();

    cv::Mat cvImage(height, width, CV_8UC1);
    memcpy(cvImage.data, imageData, width * height * sizeof(unsigned char));

    return cvImage;
}


float* SampleListener::handData(const Leap::Hand& hand, const cv::Mat& image){

  string handType = hand.isLeft() ? "Left hand" : "Right hand";
  const Vector position = hand.palmPosition();
  
  const Vector normal = hand.palmNormal();
  const Vector direction = hand.direction();

  // Calculate the hand's pitch, roll, and yaw angles
  
  float* vec = new float[3];
  vec[0] = direction.pitch()* RAD_TO_DEG;
  vec[1] = normal.roll() * RAD_TO_DEG;
  vec[2] = direction.yaw() ; 
 
  ofstream outputFile;
  outputFile.open(_output_file,ios::app);

  
  if(hand.isLeft()){

    cv::putText(image, "Left hand", cv::Point(525, 30),  cv::FONT_HERSHEY_SIMPLEX , 0.55, cv::Scalar(255, 0, 0), 1);
    if(_record){

      outputFile << _frames <<  ","      
                << vec[0] << ","
                << vec[1] <<  ","
                << vec[2] <<  ","
                << '0' <<  ","
                << '0' <<  ","
                << '0' <<  ","<<endl;
    }
  } 
  else{

    cv::putText(image, "Right hand", cv::Point(525, 50), cv::FONT_HERSHEY_SIMPLEX , 0.55, cv::Scalar(255, 0, 0), 1);
    if(_record){
      
      outputFile << _frames <<  ","
                << '0' <<  ","
                << '0' <<  ","
                << '0' <<  ","
                << vec[0] << ","
                << vec[1] <<  ","
                << vec[2] <<  ","<<endl;
    }

  }
  

  outputFile.close();

  return vec;


}

void SampleListener::setRecordStatus(bool set_record){
  _record = set_record;
}

void SampleListener::setOutputFile(const string file_name){
  _output_file = file_name;
  cout<<"Output file : "<<_output_file<<endl;
}


