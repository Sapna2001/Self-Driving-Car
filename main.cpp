#include <opencv2/opencv.hpp>
#include <raspicam_cv.h>
#include <iostream>

using namespace std;
using namespace cv;
using namespace raspicam;

int main() {
    RaspiCam_Cv Camera;
    cout << "Connecting to camera" << endl;
    if (!Camera.open()) {
        cout << "Failed to Connect" << endl;
        return -1;
    }
    
    return 0;
}
