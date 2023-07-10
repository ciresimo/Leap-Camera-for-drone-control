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

  //Create the output folder if it doesn't exist
  //I could create a smarter way to organize things
  fs::create_directory("Output");

  //Assign the outputfile name from input. The default case is "Output.txt"
  if (argc > 1){

    cout<<"set the name: "<<argv[1]<<endl;   
    listener.setOutputFile(argv[1]);

  }
  else{

    listener.setOutputFile("Output/Output.txt");

  }

  // Have the sample listener receive events from the controller
  controller.addListener(listener);


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
