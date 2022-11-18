#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;

Mat matrix, framePerspective, frameGray, frameThreshold, frameEdge, frameFinal;
Mat frame = imread("C:\\Users\\Sapna\\Desktop\\lane1.jpeg");
// VideoCapture cap(0);
Point2f Source[] = { Point2f(240, 150),Point2f(800, 150),Point2f(165, 260), Point2f(880, 260) };
Point2f Destination[] = { Point2f(300,150),Point2f(740,150),Point2f(225,260), Point2f(820,260) };

using namespace cv;

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
/*
void Capture()
{
    cap.grab();
    cap.retrieve(frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
}
*/

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

    while (true) { 
        auto start = std::chrono::system_clock::now();

        // Capture();
        Perspective();
        Threshold();

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
        */

        namedWindow("final frame", WINDOW_KEEPRATIO);
        moveWindow("final frame", 80, 100);
        resizeWindow("final frame", 640, 480);
        imshow("final frame", frameFinal);

        

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = end - start;
        float time = elapsedSeconds.count();
        int FPS = 1 / time;

        waitKey(1);
    }

    // cap.release();
    return 0;
}
