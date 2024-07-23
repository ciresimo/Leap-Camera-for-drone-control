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
  _frames_to_collect = 100;
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

  //Insert headers of the columns
  outputFile << "frame" <<  ","
              << "isLeft" << ","       //Indicates the hand is left(1) or right(0)            
              << "pitch" << ","
              << "yaw" <<  ","
              << "roll" << ","
              << "palm_x" << ","
              << "palm_y" << ","
              << "palm_z" << ","
              << "grabStrength" << "," 
              << "thumb_yaw" << "," 
              << "middle_yaw" << ","
              << "thumb_x" << ","
              << "thumb_y" << ","
              << "thumb_z" << ","
              << "middle_x" << ","
              << "middle_y"<< ","
              << "middle_z" << "\n";
  outputFile.close();
}

void SampleListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  cout << "Disconnected" << endl;
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

      // TODO: find a way to decide if is used to collect sample for training or feed data into the NN
      // handData(rightmost,cvImage);

      // TODO: find a way to handle both hands. Could be simply store values in two csv and have the network reading in alternate way
      continuousHandData(rightmost);

      
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


void SampleListener::handData(const Leap::Hand& hand, const cv::Mat& image){

  //Strem to video if the hand is left or right
  if(hand.isLeft()){

    cv::putText(image, "Left hand", cv::Point(525, 30),  cv::FONT_HERSHEY_SIMPLEX , 0.55, cv::Scalar(255, 0, 0), 1);
   
  } 
  else{

    cv::putText(image, "Right hand", cv::Point(525, 50), cv::FONT_HERSHEY_SIMPLEX , 0.55, cv::Scalar(255, 0, 0), 1);

  }
  
  const Vector position = hand.palmPosition();
  
  const Vector normal = hand.palmNormal();
  const Vector direction = hand.direction();

  // Calculate the hand's pitch, roll, and yaw angles
  
  float* vec = new float[3];
  vec[0] = direction.pitch()* RAD_TO_DEG;
  vec[1] = direction.yaw() * RAD_TO_DEG ; 
  vec[2] = normal.roll() * RAD_TO_DEG;

  // Get fingers
  const FingerList fingers = hand.fingers();

  // Consider just Thumb and Middle  

  float thumb_yaw;
  float middle_yaw;
  float* thumb_pos = new float[3];
  float* middle_pos = new float[3];


  for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
    const Finger finger = *fl;

    if(finger.type() == 0){

      thumb_yaw = atan2(finger.direction()[0],-finger.direction()[2])  ; //see https://stackoverflow.com/questions/26555040/yaw-pitch-and-roll-to-glmrotate

      thumb_pos[0] = finger.tipPosition()[0];
      thumb_pos[1] = finger.tipPosition()[1];
      thumb_pos[2] = finger.tipPosition()[2];   
            
    }
    else if(finger.type() == 2){

      middle_yaw = atan2(finger.direction()[0],-finger.direction()[2])  ;
      
      middle_pos[0] = finger.tipPosition()[0];
      middle_pos[1] = finger.tipPosition()[1];
      middle_pos[2] = finger.tipPosition()[2];

    }
  }
 
  ofstream outputFile;
  outputFile.open(_output_file,ios::app);  
  
  //If the _record variable has been set to true, print data to file
  if(_record){
    
    outputFile << _frames <<  ","
              << hand.isLeft() << ","       //Indicates the hand is left(1) or right(0)            
              << vec[0] << ","
              << vec[1] <<  ","
              << vec[2] << ","
              << position[0] << ","
              << position[1] << ","
              << position[2] << ","
              << hand.grabStrength () << "," //suggested in the paper. I think is not necessary
              << thumb_yaw << "," 
              << middle_yaw << ","
              << thumb_pos[0] << ","
              << thumb_pos[1] << ","
              << thumb_pos[2] << ","
              << middle_pos[0] << ","
              << middle_pos[1] << ","
              << middle_pos[2] << "\n";
  }
  outputFile.close();
  delete [] thumb_pos;
  delete [] middle_pos;
  delete [] vec;
  return ;
}

void SampleListener::continuousHandData(const Leap::Hand& hand) {
    static vector<vector<float>> dataBuffer; // Buffer to store 100 samples
    static const int bufferSize = 100; // Size of the buffer
    static const int numFeatures = 17; // Number of features per sample

    // Collect data for the current hand
    vector<float> data(numFeatures);
    
    // Determine if the hand is left or right
    data[1] = hand.isLeft() ? 1.0f : 0.0f;

    // Calculate the hand's pitch, roll, and yaw angles
    const Vector position = hand.palmPosition();
    const Vector normal = hand.palmNormal();
    const Vector direction = hand.direction();

    data[2] = direction.pitch() * RAD_TO_DEG;
    data[3] = direction.yaw() * RAD_TO_DEG;
    data[4] = normal.roll() * RAD_TO_DEG;

    // Store palm position
    data[5] = position[0];
    data[6] = position[1];
    data[7] = position[2];

    // Store grab strength
    data[8] = hand.grabStrength();

    // Get fingers
    const FingerList fingers = hand.fingers();
    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
        const Finger finger = *fl;
        if (finger.type() == Finger::Type::TYPE_THUMB) {
            data[9] = atan2(finger.direction()[0], -finger.direction()[2]);
            data[10] = finger.tipPosition()[0];
            data[11] = finger.tipPosition()[1];
            data[12] = finger.tipPosition()[2];
        } else if (finger.type() == Finger::Type::TYPE_MIDDLE) {
            data[13] = atan2(finger.direction()[0], -finger.direction()[2]);
            data[14] = finger.tipPosition()[0];
            data[15] = finger.tipPosition()[1];
            data[16] = finger.tipPosition()[2];
        }
    }

    // Add frame number
    data[0] = _frames;

    // Add data to buffer
    dataBuffer.push_back(data);

    // Check if the buffer is full
    if (dataBuffer.size() >= bufferSize) {
        // Save buffer to CSV
        ofstream outputFile;
        outputFile.open(_output_file, ios::app);
        for (const auto& sample : dataBuffer) {
            for (size_t i = 0; i < sample.size(); ++i) {
                outputFile << sample[i];
                if (i < sample.size() - 1) {
                    outputFile << ",";
                }
            }
            outputFile << "\n";
        }
        outputFile.close();
        
        // Clear the buffer
        dataBuffer.clear();
    }
}


void SampleListener::setRecordStatus(bool set_record){
  _record = set_record;
}

void SampleListener::setOutputFile(const string file_name){
  _output_file = file_name;
  cout<<"Output file : "<<_output_file<<endl;
}


