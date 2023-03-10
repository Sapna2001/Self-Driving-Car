#include <iostream>
#include <string.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;

Mat frame, matrix, framePerspective, frameGray, frameThreshold, frameEdge, frameFinal, ROILane, camera, image;

int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;
stringstream ss;
vector<int> histrogramLane;
Point2f Source[] = {Point2f(40,295),Point2f(596,295),Point2f(0,340), Point2f(636,340)};
Point2f Destination[] = {Point2f(180,0),Point2f(480,0),Point2f(180,478), Point2f(480,478)};

/* Resolution for image
void Setup(int argc, chr** argv, RaspiCam_Cv& Camera) {
    Camera.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv, 400));
    Camera.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 240));
    Camera.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, 60));
    Camera.set(CAP_PROP_CONTRAST, ("-co", argc, argv, 60));
    Camera.set(CAP_PROP_SATURATION, ("-sa", argc, argv, 50));
    Camera.set(CAP_PROP_GAIN, ("-g", argc, argv, 50));
    Camera.set(CAP_PROP_FPS, ("-fps", argc, argv, 100));
}

// Capture frame
void Capture(int argc, char** argv, VideoCapture &camera) {
    camera >> frame;
     
} */

// ROI and perspective vision
void Perspective() {
   
	line(image,Source[0], Source[1], Scalar(0,0,255), 2);
	line(image,Source[1], Source[3], Scalar(0,0,255), 2);
	line(image,Source[3], Source[2], Scalar(0,0,255), 2);
	line(image,Source[2], Source[0], Scalar(0,0,255), 2);
    
	
	line(image, Destination[0], Destination[1], Scalar(0, 255, 0), 2);
    	line(image, Destination[1], Destination[3], Scalar(0, 255, 0), 2);
    	line(image, Destination[3], Destination[2], Scalar(0, 255, 0), 2);
    	line(image, Destination[2], Destination[0], Scalar(0, 255, 0), 2);

    	matrix = getPerspectiveTransform(Source, Destination);
    	warpPerspective(image, framePerspective, matrix, Size(560, 478));
       imshow("perspective frame", image);
}

// Threshold operations
void Threshold() {
    cvtColor(framePerspective, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray,254 , 255, frameThreshold);
    Canny(frameGray, frameEdge, 900, 900, 3, false);
    add(frameThreshold, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
    cvtColor(frameFinal, frameFinal, COLOR_RGB2BGR);
}

// Create strips to find lane
void Histrogram() {
    histrogramLane.resize(frameFinal.size().width);
    histrogramLane.clear();

    for (int i = 0; i < frameFinal.size().width; i++) {
        ROILane = frameFinal(Rect(i, 10, 1, 100));
        divide(255, ROILane, ROILane);
        histrogramLane.push_back((int)(sum(ROILane)[0]));
    }
}

// Find the two lanes
void LaneFinder() {
    vector<int>::iterator LeftPointer;
    LeftPointer = max_element(histrogramLane.begin(), histrogramLane.begin() + 150);
    LeftLanePos = distance(histrogramLane.begin(), LeftPointer);

    vector<int>::iterator RightPointer;
    RightPointer = max_element(histrogramLane.begin() + 250, histrogramLane.end());
    RightLanePos = distance(histrogramLane.begin(), RightPointer);

    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, 240), Scalar(0, 255, 0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, 240), Scalar(0, 255, 0), 2);
}

// Find the center of the lane
void LaneCenter() {
    laneCenter = (RightLanePos - LeftLanePos) / 2 + LeftLanePos;
    frameCenter = 125;

    line(frameFinal, Point2f(laneCenter, 0), Point2f(laneCenter, 240), Scalar(0, 255, 0), 3);
    line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, 240), Scalar(255, 0, 0), 3);

    Result = laneCenter - frameCenter;
}

int main(int argc, char *argv[]) {
    
  VideoCapture videoCapture(0);
  if(!videoCapture.isOpened()) {
    std::cerr << "failed to open video capture" << endl;
    return -1;
  }
    while (1) {
        videoCapture >> image;
    imshow( "image", image );
    int c = waitKey(1);
    if(c=='x') {
      break;
    }
        Perspective();
        Threshold();
        Histrogram();
        LaneFinder();
        LaneCenter();
    
        ss.str(" ");
        ss.clear();
        ss << "Result = " << Result;
        putText(image, ss.str(), Point2f(1,50), 0,1, Scalar(0,0,255), 2);
        
      
        // Original frame
        namedWindow("orignal", WINDOW_KEEPRATIO);
        moveWindow("orignal", 50, 100);
        resizeWindow("orignal", 640, 480);
        imshow("orignal", camera);

	// Bird's eye view frame
        namedWindow("perspective", WINDOW_KEEPRATIO);
        moveWindow("perspective", 640, 100);
        resizeWindow("perspective", 640, 480);
        imshow("perspective", framePerspective);
        
       namedWindow("threshold", WINDOW_KEEPRATIO);
        moveWindow("threshold", 80, 100);
        resizeWindow("threshold", 640, 480);
        imshow("threshold", frameThreshold); 
	   
	// Gray frame
	namedWindow("gray", WINDOW_KEEPRATIO);
        moveWindow("gray", 80, 100);
        resizeWindow("gray", 640, 480);
        imshow("gray", frameGray);
	   
        
	// Canny edge detection
	namedWindow("edge", WINDOW_KEEPRATIO);
        moveWindow("edge", 80, 100);
        resizeWindow("edge", 640, 480);
        imshow("edge", frameEdge);
       // waitKey(1);
	   
	// Lane detected frame
	namedWindow("final frame", WINDOW_KEEPRATIO);
        moveWindow("final frame", 50, 560);
        resizeWindow("final frame", 640, 480);
        imshow("final frame", frameFinal);
	  
	//cout << Result << endl; 
  
        waitKey(1); 
    } 

    return 0;
}
