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
#include "KalmanFilter.h"
#include "kalmantest.h"

using namespace std;
using namespace Eigen;

int main() {

    FILE* f_in;
    FILE* f_out;
    FILE* f_out2;

    const char* cInputFileName = "../in_przes.yuv";
    const char* cOutputFileName = "../out_przes.yuv";

    // stride margin in %
    float fStrideMargin = 1.0;
    
    vector <list<ezsift::MatchPair>> vMatchPairs;

    vector<Matrix3d> vHmatrix;

    fopen_s(&f_in, cInputFileName, "rb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in %
    Frame* pframe = new Frame(1920, 1080, fStrideMargin, fStrideMargin);
    Frame* pframeNext;
    
    int iFrameCounter = 0;

    bool bfirstFrame = true;
    
    pframe->getFrame(f_in);
   
        while (!feof(f_in)) {
            
            pframeNext = new Frame(1920, 1080, fStrideMargin, fStrideMargin);
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

            cout << "liczba dopasowań: " << match_list.size() << endl;

            vHmatrix.push_back( calcHmat(match_list) );
            vMatchPairs.push_back(match_list);
            
            //**********************************************************
            //saveMatrixHtoFile(vHmatrix[iFrameCounter]);
            //iFrameCounter++;
            //**********************************************************
            
            

            // memory manage
            delete pframe;
            pframe = pframeNext;
            
        }
    
    fclose(f_in);

    delete pframe;

    
    //**********************************************************
    //vHmatrix.clear();
    //vHmatrix = readHmatrixfromFile();
    //**********************************************************

    // vectors of matrixes for single frame
    vector<Matrix3d> vT; // translation
    vector<Matrix3d> vQ; // rotation
    vector<Matrix3d> vR; // scale

    iFrameCounter = 0;

    for (int i = 0; i < vHmatrix.size(); i++) {
        matrixFactorisationH(vT, vQ, vR, vHmatrix[i]);
    }

    vector<Matrix3d> vH;

    // Kalman
        for (int i = 0; i < vHmatrix.size(); i++) {
                vH.push_back(vT[i] * vQ[i] * vR[i]);
        }
        
        vector<Matrix3d> vHkf;
        vHkf.clear();
        
        vHkf = kTestTQR(vT, vQ, vR);


    fopen_s(&f_in, cInputFileName, "rb");
    fopen_s(&f_out, cOutputFileName, "wb");

    pframe = new Frame(1920, 1080, fStrideMargin, fStrideMargin);
    pframe->getFrame(f_in);

    while (!feof(f_in)) {

        pframeNext = new Frame(1920, 1080, fStrideMargin, fStrideMargin);
        pframeNext->getFrame(f_in);

        if (feof(f_in)) {
            correctFrameByH(pframe, vHkf[iFrameCounter-1]);
            pframe->saveFrame(f_out);
            break;
        }

        cout << iFrameCounter << endl;

        correctFrameByH(pframe, vHkf[iFrameCounter]);

        pframe->saveFrame(f_out);

        // memory manage
        delete pframe;
        pframe = pframeNext;

        iFrameCounter++;

    }

    fclose(f_in);
    fclose(f_out);

    delete pframe;

    return 0;
}
