#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;

Mat matrix, framePerspective, frameGray, frameThreshold, frameEdge, frameFinal, ROILane;
Mat frame = imread("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\test_images\\lane1.jpeg");

// VideoCapture cap(0);
Point2f Source[] = { Point2f(240, 150),Point2f(800, 150),Point2f(165, 260), Point2f(880, 260) };
Point2f Destination[] = { Point2f(300,150),Point2f(740,150),Point2f(225,260), Point2f(820,260) };
vector<int> histrogramLane;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;

CascadeClassifier Stop_Cascade, Object_Cascade, Traffic_Cascade;
Mat frame_Stop = imread("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\test_images\\stop_sign_1.png");
Mat frame_Traffic = imread("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\test_images\\traffic_light_3.jpeg");
Mat ROIStop, gray_Stop, ROIObject, gray_Object, frame_Object, RoI_Traffic, gray_Traffic;
vector<Rect> Stop, Object, Traffic;

/*
void Setup(int argc, char** argv)
{
    cap.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv, 360));
    cap.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 240));
    cap.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, 50));
    cap.set(CAP_PROP_CONTRAST, ("-co", argc, argv, 50));
    cap.set(CAP_PROP_SATURATION, ("-sa", argc, argv, 50));
    cap.set(CAP_PROP_GAIN, ("-g", argc, argv, 50));
    cap.set(CAP_PROP_FPS, ("-fps", argc, argv, 100));
}
*/

// Capture frame
void Capture()
{
    // cap.grab();
    // cap.retrieve(frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
    cvtColor(frame_Stop, frame_Stop, COLOR_BGR2RGB);
}

void Perspective()
{
    
    line(frame, Source[0], Source[1], Scalar(0, 0, 255), 2);
    line(frame, Source[1], Source[3], Scalar(0, 0, 255), 2);
    line(frame, Source[3], Source[2], Scalar(0, 0, 255), 2);
    line(frame, Source[2], Source[0], Scalar(0, 0, 255), 2);

    line(frame, Destination[0], Destination[1], Scalar(0, 255, 0), 2);
    line(frame, Destination[1], Destination[3], Scalar(0, 255, 0), 2);
    line(frame, Destination[3], Destination[2], Scalar(0, 255, 0), 2);
    line(frame, Destination[2], Destination[0], Scalar(0, 255, 0), 2);
    

    matrix = getPerspectiveTransform(Source, Destination);
    warpPerspective(frame, framePerspective, matrix, Size(800, 110));
}

// Threshold operations
void Threshold()
{
    cvtColor(framePerspective, frameGray, COLOR_RGB2GRAY);
    inRange(frameGray, 220, 255, frameThreshold);
    Canny(frameGray, frameEdge, 900, 900, 3, false);
    add(frameThreshold, frameEdge, frameFinal);
    cvtColor(frameFinal, frameFinal, COLOR_GRAY2RGB);
    cvtColor(frameFinal, frameFinal, COLOR_RGB2BGR);
}

// Create strips to find lane
void Histrogram()
{
    histrogramLane.resize(frameFinal.size().width);
    histrogramLane.clear();

    for (int i = 0; i < frameFinal.size().width; i++)
    {
        ROILane = frameFinal(Rect(i, 10, 1, 100));
        divide(255, ROILane, ROILane);
        histrogramLane.push_back((int)(sum(ROILane)[0]));
    }
}

// Find the two lanes
void LaneFinder()
{
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
void LaneCenter()
{
    laneCenter = (RightLanePos - LeftLanePos) / 2 + LeftLanePos;
    frameCenter = 245;

    line(frameFinal, Point2f(laneCenter, 0), Point2f(laneCenter, 240), Scalar(0, 255, 0), 3);
    line(frameFinal, Point2f(frameCenter, 0), Point2f(frameCenter, 240), Scalar(255, 0, 0), 3);

    Result = laneCenter - frameCenter;
}

void Stop_detection()
{
    if(!Stop_Cascade.load("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\Stop_Sign\\classifier\\stop_cascade.xml"))
    {
	printf("Unable to open stop cascade file");
    }
    
    ROIStop = frame_Stop(Rect(0,0,400,270));
    cvtColor(ROIStop, gray_Stop, COLOR_RGB2GRAY);
    equalizeHist(gray_Stop, gray_Stop);
    Stop_Cascade.detectMultiScale(gray_Stop, Stop);
    
    for(int i=0; i<Stop.size(); i++)
    {
	Point P1(Stop[i].x, Stop[i].y);
	Point P2(Stop[i].x + Stop[i].width, Stop[i].y + Stop[i].height);
	
	rectangle(ROIStop, P1, P2, Scalar(0, 255, 0), 2);
	putText(ROIStop, "Stop Sign", P1, FONT_HERSHEY_SIMPLEX, 1,  Scalar(0, 0, 255, 255), 2);
    }   
}

void Object_detection()
{
    if (!Object_Cascade.load("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\obstacle\\classifier\\classifier\\obstacle_cascade.xml"))
    {
        printf("Unable to open stop cascade file");
    }

    ROIObject = frame_Object(Rect(0, 0, 1224, 1024));
    cvtColor(ROIObject, gray_Object, COLOR_RGB2GRAY);
    equalizeHist(gray_Object, gray_Object);
    Object_Cascade.detectMultiScale(gray_Object, Object);

    for (int i = 0; i < Object.size(); i++)
    {
        Point P1(Object[i].x, Object[i].y);
        Point P2(Object[i].x + Object[i].width, Object[i].y + Object[i].height);

        rectangle(ROIObject, P1, P2, Scalar(0, 255, 0), 2);
        putText(ROIObject, "Vehicle", P1, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255, 255), 2);
    }
}

void Traffic_detection()
{
    if (!Traffic_Cascade.load("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\traffic\\classifier\\Traffic_cascade.xml"))
    {
        printf("Unable to open traffic cascade file");
    }

    RoI_Traffic = frame_Traffic(Rect(0, 0, 800, 600));
    cvtColor(RoI_Traffic, gray_Traffic, COLOR_RGB2GRAY);
    equalizeHist(gray_Traffic, gray_Traffic);
    Traffic_Cascade.detectMultiScale(gray_Traffic, Traffic);

    for (int i = 0; i < Traffic.size(); i++)
    {
        Point P1(Traffic[i].x, Traffic[i].y);
        Point P2(Traffic[i].x + Traffic[i].width, Traffic[i].y + Traffic[i].height);

        rectangle(RoI_Traffic, P1, P2, Scalar(0, 0, 255), 2);
        putText(RoI_Traffic, "Red Light", P1, FONT_HERSHEY_PLAIN, 1.5, Scalar(0, 0, 255, 255), 2);

    }

}



int main(int argc, char** argv)
{

    Mat image, matrix;
    // Setup(argc, argv);

    /*
    if (!cap.isOpened()) {
        cout << "No video stream detected" << endl;
        system("pause");
        return-1;
    }*/

    Capture();

    while (true) { 
        auto start = std::chrono::system_clock::now();
        frame_Object = imread("C:\\Users\\Sapna\\Desktop\\SelfDrivingCar\\test_images\\obstacle_4.jpeg");
        cvtColor(frame_Object, frame_Object, COLOR_BGR2RGB);
        Perspective();
        Threshold();
        Histrogram();
        LaneFinder();
        LaneCenter();
        // cout << "Result: " << Result << endl;

        Stop_detection();
        // Object_detection();
        // Traffic_detection();
        // cout << frameFinal.size().width << " " << frameFinal.size().height;

      /*
        namedWindow("orignal", WINDOW_KEEPRATIO);
        moveWindow("orignal", 0, 100);
        resizeWindow("orignal", 640, 480);
        imshow("orignal", frame);
   
  
        namedWindow("perspective", WINDOW_KEEPRATIO);
        moveWindow("perspective", 640, 100);
        resizeWindow("perspective", 640, 480);
        imshow("perspective", framePerspective);


        namedWindow("gray", WINDOW_KEEPRATIO);
        moveWindow("gray", 80, 100);
        resizeWindow("gray", 640, 480);
        imshow("gray", frameGray);

        namedWindow("final frame", WINDOW_KEEPRATIO);
        moveWindow("final frame", 80, 100);
        resizeWindow("final frame", 640, 480);
        imshow("final frame", frameFinal);
        */
        namedWindow("Stop Sign frame", WINDOW_KEEPRATIO);
        moveWindow("Stop Sign frame", 80, 100);
        resizeWindow("Stop Sign frame", 800, 640);
        imshow("Stop Sign frame", ROIStop);

        /*
        namedWindow("Object frame", WINDOW_KEEPRATIO);
        moveWindow("Object frame", 80, 100);
        resizeWindow("Object frame", 1200, 1200);
        imshow("Object frame", ROIObject);
        */

        /*
        namedWindow("Traffic", WINDOW_KEEPRATIO);
        moveWindow("Traffic", 0, 100);
        resizeWindow("Traffic", 640, 480);
        imshow("Traffic", RoI_Traffic);

        /*
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = end - start;
        float time = elapsedSeconds.count();
        int FPS = 1 / time;
        */

        waitKey(1);
    }

    // cap.release();
    return 0;
}



