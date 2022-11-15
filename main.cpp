#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;
using namespace raspicam;

Mat frame, matrix, framePerspective;
RaspiCam_Cv Camera;

Point2f Source[] = { Point2f(50,200),Point2f(200,200),Point2f(0,240), Point2f(360,240) };
Point2f Destination[] = { Point2f(60,0),Point2f(300,0),Point2f(60,240), Point2f(300,240) };

// Resolution for image
void Setup(int argc, char **argv, RaspiCam_Cv &Camera)
{
    Camera.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv, 360));
    Camera.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 240));
    Camera.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, 50));
    Camera.set(CAP_PROP_CONTRAST, ("-co", argc, argv, 50));
    Camera.set(CAP_PROP_SATURATION, ("-sa", argc, argv, 50));
    Camera.set(CAP_PROP_GAIN, ("-g", argc, argv, 50));
    Camera.set(CAP_PROP_FPS, ("-fps", argc, argv, 100));
}

// Capture frame
void Capture()
{
    Camera.grab();
    Camera.retrieve(frame);
    cvtColor(frame, frame, COLOR_BGR2RGB);
}

// ROI and perspective vision
void Perspective()
{
	line(frame,Source[0], Source[1], Scalar(0,0,255), 2);
	line(frame,Source[1], Source[3], Scalar(0,0,255), 2);
	line(frame,Source[3], Source[2], Scalar(0,0,255), 2);
	line(frame,Source[2], Source[0], Scalar(0,0,255), 2);
	
	line(frame, Destination[0], Destination[1], Scalar(0, 255, 0), 2);
    	line(frame, Destination[1], Destination[3], Scalar(0, 255, 0), 2);
    	line(frame, Destination[3], Destination[2], Scalar(0, 255, 0), 2);
    	line(frame, Destination[2], Destination[0], Scalar(0, 255, 0), 2);

    	matrix = getPerspectiveTransform(Source, Destination);
    	warpPerspective(frame, framePerspective, matrix, Size(360, 240));
}

int main(int argc, char **argv)
{
    Setup(argc, argv, Camera);

    cout << "Connecting to camera" << endl;
    if (!Camera.open())
    {
        cout << "Failed to Connect" << endl;
        return -1;
    }

    while (1)
    {
        auto start = std::chrono::system_clock::now();

        Capture();
        Perspective();

        namedWindow("orignal", WINDOW_KEEPRATIO);
        moveWindow("orignal", 50, 100);
        resizeWindow("orignal", 640, 480);
        imshow("orignal", frame);

        namedWindow("perspective", WINDOW_KEEPRATIO);
        moveWindow("perspective", 500, 100);
        resizeWindow("perspective", 640, 480);
        imshow("perspective", framePerspective);

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = end - start;
        float time = elapsedSeconds.count();
        int FPS = 1 / t;

        waitKey(1);
    }

    return 0;
}
