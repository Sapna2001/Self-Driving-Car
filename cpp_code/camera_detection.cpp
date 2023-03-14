#include <iostream>
#include <string.h>
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;
//VideoCapture camera = new Videocapture(0);
Mat frame;


int main(int argc, char *argv[])
{
  Mat image;
  VideoCapture videoCapture(0);
  if(!videoCapture.isOpened()) {
    std::cerr << "failed to open video capture" << endl;
    return -1;
  }
  while(1) {
    videoCapture >> image;
    imshow( "image", image );
    int c = waitKey(1);
    if(c=='x') {
      break;
    }
  }
  return 0;
} 

     /* Mat output;
        VideoCapture cap(0);


        if( !cap.isOpened() )
        {
            cout << "Could not initialize capturing...\n";
            return 0;
        }


        while(1){
            cap >> output;

            imshow("webcam input", output);
            char c = (char)waitKey(10);
            if( c == 27 ) break;
          } */
    
