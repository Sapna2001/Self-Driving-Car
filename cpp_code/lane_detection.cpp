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
#include <wiringPi.h>
#include <SerialStream.h>
#include <unistd.h>

using namespace std;
using namespace cv;

Mat frame, matrix, framePerspective, frameGray, frameThreshold, frameEdge, frameFinal, ROILane;
//Mat Camera;
Mat camera;
Mat image;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;
stringstream ss;
vector<int> histrogramLane;
Point2f Source[] = {Point2f(6,280),Point2f(630,280),Point2f(-30,315), Point2f(666,315)};
Point2f Destination[] = {Point2f(150,0),Point2f(480,0),Point2f(150,478), Point2f(480,478)};

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
       imshow("Original frame", image);
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
    LeftPointer = max_element(histrogramLane.begin(), histrogramLane.begin() + frameFinal.size().width/2);
    LeftLanePos = distance(histrogramLane.begin(), LeftPointer);

    vector<int>::iterator RightPointer;
    RightPointer = max_element(histrogramLane.begin() + frameFinal.size().width/2, histrogramLane.end());
    RightLanePos = distance(histrogramLane.begin(), RightPointer);

    line(frameFinal, Point2f(LeftLanePos, 0), Point2f(LeftLanePos, frameFinal.size().height), Scalar(0, 255, 0), 2);
    line(frameFinal, Point2f(RightLanePos, 0), Point2f(RightLanePos, frameFinal.size().height), Scalar(0, 255, 0), 2);
}


// Find the center of the lane
void LaneCenter() {
    laneCenter = (RightLanePos - LeftLanePos) / 2 + LeftLanePos;
    frameCenter = 314;

    line(frameFinal, Point2f(laneCenter, 0), Point2f(laneCenter, frameFinal.size().height), Scalar(0, 255, 0), 3);
    line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, frameFinal.size().height), Scalar(255, 0, 0), 3);

    Result = laneCenter - frameCenter;
}

void communicate(string ch) {
    using namespace std;
    using namespace LibSerial;

    //cout << "Running. Press CTRL-C to exit." << endl;
    SerialStream arduino;
    arduino.Open("/dev/ttyACM0");
    arduino.SetBaudRate(SerialStreamBuf::BAUD_9600);
    arduino.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    arduino.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_NONE);
    arduino.SetParity(SerialStreamBuf::PARITY_NONE);
    arduino.SetNumOfStopBits(1);
    arduino.SetVTime(100); // inter-character timeout in deciseconds

    if (arduino.IsOpen())
    {
        cout << "/dev/ttyACM0 connected!" << endl;
        try
        {
            
                cout << "Enter command: ";
                string cmd = ch;
                arduino << cmd << endl;
                //usleep(100000); // wait for arduino to answer
                while (arduino.rdbuf()->in_avail() == 0) {} // wait for data to be available
                if (arduino.rdbuf()->in_avail() > 0)
                {
                    string answer;
                    getline(arduino, answer);
                    cout << answer << endl;
                    arduino.ignore(1000, '\n'); // remove data after reading
                }
            
        }
        catch (const exception& e)
        {
            cout << "Exception: " << e.what() << endl;
        }
        catch (...)
        {
            cout << "Unknown exception occurred." << endl;
        }
    }
    else
    {
        cout << "Failed to open /dev/ttyACM0" << endl;
    }

}

int main(int argc, char *argv[]) {
    
  VideoCapture videoCapture("outcpp.avi");
  if(!videoCapture.isOpened()) {
    std::cerr << "failed to open video capture" << endl;
    return -1;
  }
    while (1) {
        videoCapture.set(CAP_PROP_FPS, ("-fps",1));
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
        
        
        if (Result == 0)
    {
	
    communicate("a");
	cout<<"Forward"<<endl;
    }
    
        
    else if (Result >10 && Result <20)
    {
	communicate("c");
	cout<<"Left1"<<endl;
    }
    
        else if (Result >=20 && Result <30)
    {
	communicate("d");
	cout<<"Left2"<<endl;
    }
    
        else if (Result >40)
    {
	communicate("e");
	cout<<"Left3"<<endl;
    }
    
        else if (Result <-10 && Result >-20)
    {
	communicate("f");
	cout<<"Right1"<<endl;
    }
    
        else if (Result <=-20 && Result >-30)
    {
	communicate("g");
	cout<<"Right2"<<endl;
    }
    
        else if (Result <-30)
    {
	communicate("h");
            cout<<"Right3"<<endl;

	//cout<<"Left3"<<endl;
    } else {
        communicate("a");
    }
    
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

    // Threshold framr      
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
	  
	cout << Result << endl;
    
        waitKey(1); 
    } 

    return 0;
}
