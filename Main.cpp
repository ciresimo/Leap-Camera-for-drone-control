#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp>
#include "Leap.h"
#include "SampleListener.h"


using namespace Leap;
using namespace std;

int main(int argc, char** argv) {
  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  cout<<"I modified this"<<endl;

  // Create an OpenCV window
  cv::namedWindow("Leap Motion Image", cv::WINDOW_NORMAL);  

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  //Allow the stream of immages
  controller.setPolicy(Leap::Controller::POLICY_IMAGES);

  //Checks if the programm must be run in background
  if (argc > 1 && strcmp(argv[1], "--bg") == 0){

    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES); 

  }
  cout << "Press 's' to start recording" << endl;
  cout << "Press 'p' to stop recording" << endl;
  cout << "Press 'q' to exit the program" << endl;

  // // Keep this process running until Enter is pressed
  cout << "Press Enter to quit..." << endl;
  cin.get();
  
  

  // Remove the sample listener when done
  controller.removeListener(listener);

  // Close the OpenCV window
  cv::destroyAllWindows();

  return 0;
}
