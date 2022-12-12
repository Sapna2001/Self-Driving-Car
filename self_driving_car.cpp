#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;

stringstream ss;

Mat frame_Traffic = imread("D:\\PROJECT\\traffic_light_data\\red\\red11.jpg");
//Machine Learning variables
CascadeClassifier Traffic_Cascade;
Mat RoI_Traffic, gray_Traffic;
vector<Rect> Traffic;
int dist_Traffic;


void Traffic_detection()
{
    if (!Traffic_Cascade.load("D://PROJECT//traffic_light_data//classifier//Traffic_cascade.xml"))
    {
        printf("Unable to open traffic cascade file");
    }

    RoI_Traffic = frame_Traffic(Rect(200,140,200,140));
    //RoI_Traffic = frame_Traffic(Rect(100, 35, 110, 300)); For red14.jpg
    cvtColor(RoI_Traffic, gray_Traffic, COLOR_RGB2GRAY);
    equalizeHist(gray_Traffic, gray_Traffic);
    Traffic_Cascade.detectMultiScale(gray_Traffic, Traffic);

    for (int i = 0; i < Traffic.size(); i++)
    {
        Point P1(Traffic[i].x, Traffic[i].y);
        Point P2(Traffic[i].x + Traffic[i].width, Traffic[i].y + Traffic[i].height);

        //rectangle(RoI_Traffic, P1, P2, Scalar(0, 0, 255), 2);
        putText(RoI_Traffic, "Traffic Light", P1, FONT_HERSHEY_PLAIN, 0.8, Scalar(0, 0, 255, 255), 2);
    

    }

}


int main(int argc, char** argv)
{
   
    while (1)
    {

        auto start = std::chrono::system_clock::now();
       
        Traffic_detection();

        namedWindow("Traffic", WINDOW_KEEPRATIO);
        moveWindow("Traffic", 0, 100);
        resizeWindow("Traffic", 640, 480);
        imshow("Traffic", RoI_Traffic);


        waitKey(1);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;

        //float t = elapsed_seconds.count();
        //int FPS = 1 / t;
        //cout<<"FPS = "<<FPS<<endl;

    }


    return 0;

}
