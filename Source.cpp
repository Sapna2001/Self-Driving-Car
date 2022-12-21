#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;

Mat matrix, framePerspective, frameGray, frameThreshold, frameEdge, frameFinal, ROILane;
//Mat frame = imread("C:\\Users\\user\\Desktop\\lane1.jpeg");


Point2f Source[] = { Point2f(240, 150),Point2f(800, 150),Point2f(165, 260), Point2f(880, 260) };
Point2f Destination[] = { Point2f(300,150),Point2f(740,150),Point2f(225,260), Point2f(820,260) };
vector<int> histrogramLane;
int LeftLanePos, RightLanePos, frameCenter, laneCenter, Result;

CascadeClassifier Object_Cascade;
//Mat frame_Object = imread("C:\\Users\\user\\Desktop\\pos\\vehicle.jpeg"); 
Mat frame_Object;
//= imread("C:\\Users\\user\\Desktop\\pos\\vehicle2.jpeg");

Mat ROIObject, gray_Object;
vector<Rect> Object;

// Capture frame
/*void Capture()
{
    // cap.grab();
    // cap.retrieve(frame);
    //cvtColor(frame, frame, COLOR_BGR2RGB);
    cvtColor(frame_Object, frame_Object, COLOR_BGR2RGB);
    //cvtColor(frame, frame, COLOR_BGR2RGB);
}*/

/*void Perspective()
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
}*/

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

void Object_detection()
{
    if (!Object_Cascade.load("C:\\Users\\user\\Desktop\\obstacle\\classifier\\cascade.xml"))
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


int main(int argc, char** argv)
{

    Mat image, matrix;
    //string path = "Resources/test.png";
    //Mat img = imread(path);

    

    while (true) {
        //Capture();
        //cvtColor(frame_Object, frame_Object, COLOR_BGR2RGB);
        frame_Object = imread("C:\\Users\\user\\Desktop\\xyz\\9.jpeg");
        cvtColor(frame_Object, frame_Object, COLOR_BGR2RGB);
        auto start = std::chrono::system_clock::now();
       // Perspective();
       // Threshold();
        //Histrogram();
      //  LaneFinder();
       // LaneCenter();
        cout << "Vehicle detected"<< endl;

        Object_detection();
        

        namedWindow("Object frame", WINDOW_KEEPRATIO);
        //moveWindow("Object frame", 640, 580);
        resizeWindow("Object frame", 1200, 1200);
        imshow("Object frame", ROIObject);


       
        waitKey(1);
    }

    return 0;
}