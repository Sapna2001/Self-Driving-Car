#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <ctime>

using namespace std;
using namespace cv;


Mat frame, image;

/*void Setup(int argc, char** argv, RaspiCam_Cv& Camera) {
    Camera.set(CAP_PROP_FRAME_WIDTH, ("-w", argc, argv, 400));
    Camera.set(CAP_PROP_FRAME_HEIGHT, ("-h", argc, argv, 240));
    Camera.set(CAP_PROP_BRIGHTNESS, ("-br", argc, argv, 50));
    Camera.set(CAP_PROP_CONTRAST, ("-co", argc, argv, 50));
    Camera.set(CAP_PROP_SATURATION, ("-sa", argc, argv, 50));
    Camera.set(CAP_PROP_GAIN, ("-g", argc, argv, 50));
    Camera.set(CAP_PROP_FPS, ("-fps", argc, argv, 100));

}*/


int main(int argc, char** argv) {

    VideoCapture videoCapture(0);
  if(!videoCapture.isOpened()) {
    std::cerr << "failed to open video capture" << endl;
    return -1;
  }
    

    for (int i=0; i<100; i++) {
	
	       //   while (1) {
        videoCapture >> image;
   // imshow( "image", image );
   /* int c = waitKey(1);
    if(c=='x') {
      break;
    } */
		 
		  cvtColor(image,image, COLOR_BGR2GRAY);
		  imshow("Frame", image);
		  imwrite("Stop"+to_string(i)+".jpg" , image);
		  waitKey();
	  //}
      }

    return 0;
}
