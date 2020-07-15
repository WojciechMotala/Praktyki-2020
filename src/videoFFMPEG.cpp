#include <iostream>
#include <Eigen/Dense>
#include <math.h>
#include <vector>
#include <array>
#include <fstream>

#include "ezsift.h"
#include "Frame.h"
#include "frameHelpers.h"
#include "helpers.h"

using namespace std;
using namespace Eigen;

int main() {

    FILE* f_in;
    FILE* f_out;

    const char* cInputFileName = "../in_srednie.yuv";
    const char* cOutputFileName = "../out.yuv";
    
    vector <array<float, 3>> vRTdata;
    vector <list<ezsift::MatchPair>> vMatchPairs;

    vector<Matrix3f> vHmatrix;

    array<float, 3> frameRT;

    // init for first frame
    Matrix3f firstFrameH;
    firstFrameH << 0, 0, 0, 0, 0, 0, 0, 0, 0;
    vHmatrix.push_back(firstFrameH);

    fopen_s(&f_in, cInputFileName, "rb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in %
    Frame* pframe = new Frame(1920, 1080, 1.0, 1.0);
    Frame* pframeNext;
    
    ofstream myfile;

    bool bfirstFrame = true;
    
    pframe->getFrame(f_in);
   
        while (!feof(f_in)) {
            
            pframeNext = new Frame(1920, 1080, 1.0, 1.0);
            pframeNext->getFrame(f_in);

            if (feof(f_in))
                break;

            ezsift::Image<uint8_t> imageFirst;
            ezsift::Image<uint8_t> imageSecond;

            const uint8_t* pframeBufY = pframe->getBufY();
            int iframeWidth = pframe->getWidthY();
            int iframeHeight = pframe->getHeightY();
            int iframeStrideWidthY = pframe->getStrideWidthY();

            const uint8_t* pframeNextBufY = pframeNext->getBufY();
            int iframeNextWidth = pframeNext->getWidthY();
            int iframeNextHeight = pframeNext->getHeightY();
            int iframeNextStrideWidthY = pframeNext->getStrideWidthY();

            // Detect keypoints
            list<ezsift::SiftKeypoint> kpt_list_first;
            list<ezsift::SiftKeypoint> kpt_list_second;
            list<ezsift::MatchPair> match_list;
            
            bool bExtractDescriptor = true;

            imageFirst.read_pgm_direct(pframeBufY, iframeWidth, iframeHeight, iframeStrideWidthY);
            imageSecond.read_pgm_direct(pframeNextBufY, iframeNextWidth, iframeNextHeight, iframeNextStrideWidthY);

            sift_cpu(imageFirst, kpt_list_first, bExtractDescriptor);
            sift_cpu(imageSecond, kpt_list_second, bExtractDescriptor);

            // Match keypoints.
            match_keypoints(kpt_list_first, kpt_list_second, match_list);

            vHmatrix.push_back( calcHmat(match_list) );

            // memory manage
            delete pframe;
            pframe = pframeNext;
            
        }
    
    fclose(f_in);

    delete pframe;

    fopen_s(&f_in, cInputFileName, "rb");
    fopen_s(&f_out, cOutputFileName, "wb");

    pframe = new Frame(1920, 1080, 1.0, 1.0);
    pframe->getFrame(f_in);

    int iFrameCounter = 0;

    while (!feof(f_in)) {

        iFrameCounter++;

        pframeNext = new Frame(1920, 1080, 1.0, 1.0);

        pframeNext->getFrame(f_in);

        if (feof(f_in))
            break;

        correctFrameByH(pframeNext, vHmatrix[iFrameCounter]);

        // save frame to output file
        if (bfirstFrame) {
            pframe->saveFrame(f_out);
            bfirstFrame = false;
        }

        pframeNext->saveFrame(f_out);

        // memory manage
        delete pframe;
        pframe = pframeNext;

        // matrix mul
        Matrix3f tempH = vHmatrix[iFrameCounter] * vHmatrix[iFrameCounter + 1];
        vHmatrix[iFrameCounter + 1] = tempH;

    }

    fclose(f_in);
    fclose(f_out);

    delete pframe;

    

    

    return 0;
}
