#include<opencv2/opencv.hpp>
#include<iostream>

using namespace std;
using namespace cv;

int main()
{
    const string file = "abc.mp4";
    string inputFileName;
    string outputFileName;

    cout << "Enter file name: \n >";
    cin >> inputFileName;

    cout << "Enter output file name: \n >";
    cin >> outputFileName;

    VideoCapture cap(inputFileName);

    if (!cap.isOpened()) {
        cout << "Error opening video file" << endl;
        return -1;
    }

    int frameWidth = cap.get(CAP_PROP_FRAME_WIDTH);
    int frameHeight = cap.get(CAP_PROP_FRAME_HEIGHT);
    double videoFps = cap.get(CAP_PROP_FPS);
    
    cout << "\n\n*************************" << endl;
    cout << "frame width: " << frameWidth << endl;
    cout << "frame height: " << frameHeight << endl;
    cout << "FPS: " << videoFps << endl;
    cout << "*************************\n\n" << endl;

    VideoWriter videoWrite(outputFileName, VideoWriter::fourcc('M', 'P', 'G', '4'), videoFps, Size(frameWidth, frameHeight));

    while (true) {

        Mat frame;
        cap >> frame;

        if (frame.empty())
            break;

        //uchwyt do pixela
        unsigned char* p;

        for (int x = 200; x < 500; x++) {
            for (int y = 100; y < 800; y++) {
                p = frame.ptr(y, x);
                p[0] = 0;   //B
                p[1] = 130;   //G
                p[2] = 0;   //R
            }
        }

        videoWrite.write(frame);

    }

    cap.release();
    videoWrite.release();

    return 0;
}

