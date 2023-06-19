#include <iostream>
#include <string.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <chrono>
#include <ctime>
#include <SerialStream.h>
#include <unistd.h>


using namespace std;
using namespace cv;

Mat frame, matrix, framePerspective, frameGray, frameThreshold, frameEdge, frameFinal, ROILane;

Mat image;

CascadeClassifier Stop_Cascade, Object_Cascade, Traffic_Cascade, Post_Cascade;
Mat frame_Stop, RoI_Stop, gray_Stop, frame_Object, RoI_Object, gray_Object, frame_Traffic, RoI_Traffic, gray_Traffic, gray_Post;
vector<Rect> Stop, Object, Traffic, Post;
int dist_Stop, dist_Object, dist_Traffic, dist_Post;

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
       // imshow("Original frame", image);
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
    frameCenter = 340;

    line(frameFinal, Point2f(laneCenter, 0), Point2f(laneCenter, frameFinal.size().height), Scalar(0, 255, 0), 3);
    line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, frameFinal.size().height), Scalar(255, 0, 0), 3);

    Result = laneCenter - frameCenter;
}

void Object_detection()
{
    if(!Object_Cascade.load("//home//pi3//Desktop//MACHINE LEARNING//obstacle_cascade.xml"))
    {
	printf("Unable to open Object cascade file");
    }
   
    
    //RoI_Object = frame_Object(Rect(0,0,image.size().width, image.size().height));
    cvtColor(image, gray_Object, COLOR_RGB2GRAY);
    equalizeHist(gray_Object, gray_Object);
    Object_Cascade.detectMultiScale(gray_Object, Object);
    
    for(int i=0; i<Object.size(); i++)
    {
	Point P1(Object[i].x, Object[i].y);
	Point P2(Object[i].x + Object[i].width, Object[i].y + Object[i].height);
	
	rectangle(image, P1, P2, Scalar(0, 0, 255), 2);
	putText(image, "Object", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
    imshow("Object frame", image);
	dist_Object = (-0.667)*(P2.x-P1.x) + 135.33;
	cout<<dist_Object<<"cm obj";
    /*   ss.str(" ");
       ss.clear();
       ss<<"D = "<<dist_Object<<"cm";
       putText(image, ss.str(), Point2f(120,130), 0,1, Scalar(0,0,255), 2);
	*/
    }
    
}

void Post_detection()
{
    if(!Post_Cascade.load("//home//pi3//Desktop//MACHINE LEARNING//Post_cascade.xml"))
    {
	printf("Unable to open Post cascade file");
    }
   
    
    //RoI_Object = frame_Object(Rect(0,0,image.size().width, image.size().height));
    cvtColor(image, gray_Post, COLOR_RGB2GRAY);
    equalizeHist(gray_Post, gray_Post);
    Object_Cascade.detectMultiScale(gray_Post, Post);
    
    for(int i=0; i<Post.size(); i++)
    {
	Point P1(Post[i].x, Post[i].y);
	Point P2(Post[i].x + Post[i].width, Post[i].y + Post[i].height);
	
	rectangle(image, P1, P2, Scalar(0, 0, 255), 2);
	putText(image, "Object", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
    //imshow("Object frame", image);
	dist_Post = (-0.667)*(P2.x-P1.x) + 135.33;
	cout<<dist_Post<<"cm post";
       /*ss.str(" ");
       ss.clear();
       ss<<"D = "<<dist_Object<<"cm";
       putText(RoI_Object, ss.str(), Point2f(1,130), 0,1, Scalar(0,0,255), 2);
	*/
    }
    
}

void Stop_detection()
{
    if(!Stop_Cascade.load("//home//pi3//Desktop//MACHINE LEARNING//stop_cascade.xml"))
    {
	printf("Unable to open Stop cascade file");
    }
   
    
    //RoI_Object = frame_Object(Rect(0,0,image.size().width, image.size().height));
    cvtColor(image, gray_Stop, COLOR_RGB2GRAY);
    equalizeHist(gray_Stop, gray_Stop);
    Stop_Cascade.detectMultiScale(gray_Stop, Stop);
    
    for(int i=0; i<Stop.size(); i++)
    {
	Point P1(Stop[i].x, Stop[i].y);
	Point P2(Stop[i].x + Stop[i].width, Stop[i].y + Stop[i].height);
	
	rectangle(image, P1, P2, Scalar(0, 0, 255), 2);
	putText(image, "Stop", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
    imshow("Stop frame", image);
	dist_Stop = (-0.851)*(P2.x-P1.x) + 94.407;
	cout<<dist_Stop<<"cm stop";
     /*  ss.str(" ");
       ss.clear();
       ss<<"D = "<<dist_Object<<"cm";
       putText(RoI_Object, ss.str(), Point2f(1,130), 0,1, Scalar(0,0,255), 2);
	*/
    }
    
}

void Traffic_detection()
{
    if(!Traffic_Cascade.load("//home//pi3//Desktop//MACHINE LEARNING//Traffic_cascade.xml"))
    {
	printf("Unable to open Stop cascade file");
    }
   
    RoI_Traffic=image;
    //RoI_Object = frame_Object(Rect(0,0,image.size().width, image.size().height));
    cvtColor(RoI_Traffic, gray_Traffic, COLOR_RGB2GRAY);
    equalizeHist(gray_Traffic, gray_Traffic);
    Traffic_Cascade.detectMultiScale(gray_Traffic, Traffic);
    
    for(int i=0; i<Traffic.size(); i++)
    {
	Point P1(Traffic[i].x, Traffic[i].y);
	Point P2(Traffic[i].x + Traffic[i].width, Traffic[i].y + Traffic[i].height);
	
	rectangle(RoI_Traffic, P1, P2, Scalar(0, 0, 255), 2);
	putText(RoI_Traffic, "Red", P1, FONT_HERSHEY_PLAIN, 1,  Scalar(0, 0, 255, 255), 2);
    imshow("Traffic light frame", RoI_Traffic);
	dist_Traffic = (-0.59)*(P2.x-P1.x) + 46.64;
	cout<<dist_Traffic<<"cm traffic";
   /*  ss.str(" ");
       ss.clear();
       ss<<"D = "<<dist_Traffic<<"cm";
       putText(RoI_Traffic, ss.str(), Point2f(1,130), 0,1, Scalar(0,0,255), 2);
	*/
    }
    
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
    
    
  VideoCapture videoCapture(0);
  if(!videoCapture.isOpened()) {
    std::cerr << "failed to open video capture" << endl;
    return -1;
  }
    while (1) {
        videoCapture.set(CAP_PROP_FPS, ("-fps", argc, argv, 10));
        videoCapture >> image;
    //imshow( "image", image );
    int c = waitKey(1);
    if(c=='x') {
      break;
    }
        Perspective();
        //Threshold();
        //Histrogram();
        //LaneFinder();
        //LaneCenter(); 
        Object_detection();
       // Post_detection();
       // Stop_detection();
      //  Traffic_detection();
      
    if (dist_Object > 0 && dist_Object < 50 || dist_Stop > 5 && dist_Stop < 30||dist_Traffic > 5 && dist_Traffic < 30)
    {
        communicate("s");
        }else{
            communicate("a");
            }

    if (dist_Stop > 5 && dist_Stop < 20)
    {
        communicate("s");
        }else{
            communicate("a");
            } 
    if (dist_Traffic > 5 && dist_Traffic < 20)
    {
        communicate("s");
        }else{
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
       
    // Object detected frame
	    namedWindow("Object frame", WINDOW_KEEPRATIO);
        moveWindow("Object frame", 650, 560);
        resizeWindow("Object frame", 640, 480);
        imshow("Object frame", frame_Object);
       
	//cout << Result << endl; 
   // imshow("frame", image);
  
        
  
        waitKey(1); 
    } 

    return 0;
}
