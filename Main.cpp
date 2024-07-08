#include <iostream>
#include <cstring>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "Leap.h"
#include "SampleListener.h"


using namespace Leap;
using namespace std;

int main(int argc, char** argv) {

  // Create a sample listener and controller
  SampleListener listener;
  Controller controller;

  //Get information from input:
  // - subject
  // - gesture
  // - rep

  //Always referr to the documentation provided for collecting the data

  //Store the .csv file in Output/gesture/subject

  if (argc == 4){

    //This is used if user wants to collect data for training
   
    string input1 = argv[1];  
    string input2 = argv[2];
    string input3 = argv[3];
    
    string file_repository = "Output/" + input2 +"/subj_" + input1 ;
    string file_name = file_repository +"/" + "subj_" + input1 + "_ges_" + input2 + "_rep_" + input3 + ".csv";
    filesystem::create_directories(file_repository);
    listener.setOutputFile(file_name);

  }
  else{
    
    // This is used a run time by the application
    filesystem::create_directory("Output");
    listener.setOutputFile("Output/Output.csv");

  }

  // Have the sample listener receive events from the controller
  controller.addListener(listener);

  //Set the mode of use as Head-mounted. If the camera is instead placed on the table, simply comment this line
  // controller.setPolicy(Leap::Controller::POLICY_OPTIMIZE_HMD);

  //Allow the stream of immages
  controller.setPolicy(Leap::Controller::POLICY_IMAGES);

  // Create an OpenCV window
  cv::namedWindow("Leap Motion Image", cv::WINDOW_NORMAL);  

  
  //Keep this process running until '3' is pressed from keyboard
  while(!listener._stop_program){

  }  

  // Remove the sample listener when done
  controller.removeListener(listener);

  // Close the OpenCV window
  cv::destroyAllWindows();

  cout<<"End of main"<<endl;

  return 0;
}
