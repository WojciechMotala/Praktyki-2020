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

    const char* cInputFileName = "../in_srednie.yuv";
    const char* cOutputFileName = "../out_srednie.yuv";

    // stride margin in %
    float fStrideMargin = 5.0;
    
    bool bOnlyPoints = false;
    const char* cOutputFileNameOnlyPoints = "../out_punkty.yuv";
    
    vector <array<float, 3>> vRTdata;
    vector <list<ezsift::MatchPair>> vMatchPairs;

    vector<Matrix3d> vHmatrix;

    array<float, 3> frameRT;

    // init for first frame
    //Matrix3f firstFrameH;
    //firstFrameH << 0, 0, 0, 0, 0, 0, 0, 0, 0;
    //vHmatrix.push_back(firstFrameH);

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

            /*

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
            saveMatrixHtoFile(vHmatrix[iFrameCounter]);
            iFrameCounter++;
            //**********************************************************
            
            */
            

            // memory manage
            delete pframe;
            pframe = pframeNext;
            
        }
    
    fclose(f_in);

    delete pframe;

    
    //**********************************************************
    vHmatrix.clear();
    vHmatrix = readHmatrixfromFile();
    //**********************************************************

    // vectors of matrixes for single frame
    //vector<Matrix3f> vHmean;

    vector<Matrix3d> vT; // translation
    vector<Matrix3d> vQ; // rotation
    vector<Matrix3d> vR; // scale
    //vector<Matrix3f> vMeanT;
    //vector<Matrix3f> vMeanQ;
    //vector<Matrix3f> vMeanR;

    iFrameCounter = 0;

    //Matrix3f matrixZero;
    //matrixZero << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

    //vT.push_back(matrixZero);
    //vQ.push_back(matrixZero);
    //vR.push_back(matrixZero);

    for (int i = 0; i < vHmatrix.size(); i++) {
        matrixFactorisationH(vT, vQ, vR, vHmatrix[i]);
    }

    vector<Matrix3d> vH;
    //vector<Matrix3d> vH3f;


    // Kalman
        for (int i = 0; i < vHmatrix.size(); i++) {
            //vH.push_back(matrixHnorm(vT[i] * vQ[i] * vR[i]));
                vH.push_back(vT[i] * vQ[i] * vR[i]);
                
         
        }
        
        vector<Matrix3d> vHkf;
        vHkf.clear();
        
        //KALMAN
        vHkf = kTest(vH);
        
        
        // KALMAN DO PIERWSZEJ RAMKI 
        /*
        
        MatrixXd R = getCovFromH(vH) * 1e-2;
        Eigen::Matrix< double, 6, 1> v;
        v << 1e-8, 1e-7, 4e-3, 1e-7, 1e-8, 4e-3;
        MatrixXd Q = v.array().matrix().asDiagonal();
        v << 1.0, 1.0, 1.0, 1.0, 1.0, 1.0;
        MatrixXd P = v.array().matrix().asDiagonal();
        MatrixXd F = v.array().matrix().asDiagonal();
        MatrixXd H = v.array().matrix().asDiagonal();
        MatrixXd X(6, 1);
        X.setZero();

        KalmanFilter kf(X, F, H, P, Q, R);

        MatrixXd cumulativeTransform(2, 3);
        cumulativeTransform << 1.0, 0.0, 0.0, 0.0, 1.0, 0.0;
        
        MatrixXd lastAffine = cumulativeTransform;
        MatrixXd cumulativeSmoothed = cumulativeTransform;

        
        //MatrixXd tmp(2, 3);
        //tmp << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;
        //vHkf.push_back(tmp);

        for (int i = 0; i < vH.size(); i++) {
            
            cumulativeTransform = sum2affine(cumulativeTransform, vH[i]);
            //cout << "affine:" << endl << vH[i] << endl << endl;
            MatrixXd z(6, 1);
            z(0, 0) = vH[i](0, 0); //q1
            z(1, 0) = vH[i](0, 1); //q2
            z(2, 0) = vH[i](0, 2); //t1
            z(3, 0) = vH[i](1, 0); //q3
            z(4, 0) = vH[i](1, 1); //q4
            z(5, 0) = vH[i](1, 2); //t2

            MatrixXd x1(6, 1);
            x1 = kf.predictAndUpdate(z);
            
            //cout << i << endl << endl << x1 << endl << endl;
            
            MatrixXd smoothedAffineMotion(2, 3);
            smoothedAffineMotion = reshape(x1);
            


            //cout << i << endl << endl << smoothedAffineMotion << endl << endl;
            //cout << "cumulativeTransform:" << endl << cumulativeTransform << endl << endl;

            MatrixXd affine_motion = compensatingTransform(smoothedAffineMotion, cumulativeTransform);
            //MatrixXd tmp = affine_motion.inverse();
            

            
            //cout << "smoothedAffineMotion:" << endl << smoothedAffineMotion << endl << endl;
            //cout << "AffineMotion:" << endl << affine_motion << endl << endl;
            //cout << "=============================" << endl << endl;
            
            
            vHkf.push_back(affine_motion);
            

        }
        */

    iFrameCounter = 0;
    

    fopen_s(&f_in, cInputFileName, "rb");
    fopen_s(&f_out, cOutputFileName, "wb");
    
    fopen_s(&f_out2, cOutputFileNameOnlyPoints, "wb");

    pframe = new Frame(1920, 1080, fStrideMargin, fStrideMargin);
    pframe->getFrame(f_in);

    iFrameCounter = 0;

    while (!feof(f_in)) {

        

        pframeNext = new Frame(1920, 1080, fStrideMargin, fStrideMargin);
        Frame* pframeNextCopy = new Frame();

        pframeNext->getFrame(f_in);
        pframeNextCopy->FrameCopy(*pframeNext);

        if (feof(f_in))
            break;


        if (bOnlyPoints) {
            clearImage(pframeNextCopy);
            drawSquare(pframeNextCopy, vMatchPairs[iFrameCounter], false);
            //correctFrameByH(pframeNextCopy, vHmatrix[iFrameCounter]);
            //correctFrameByH(pframeNext, vHmean[iFrameCounter]);
        }


        cout << iFrameCounter << endl;
        /*
        Matrix3d H;
        H(0, 0) = vHkf[iFrameCounter](0, 0);
        H(0, 1) = vHkf[iFrameCounter](0, 1);
        H(0, 2) = vHkf[iFrameCounter](0, 2);
        H(1, 0) = vHkf[iFrameCounter](1, 0);
        H(1, 1) = vHkf[iFrameCounter](1, 1);
        H(1, 2) = vHkf[iFrameCounter](1, 2);
        H(2, 0) = 0.0;
        H(2, 1) = 0.0;
        H(2, 2) = 1.0;
        */


        //correctFrameByH(pframeNext, H);
        correctFrameByH(pframeNext, vHkf[iFrameCounter]);
        



        // save frame to output file
        if (bfirstFrame) {

            pframe->saveFrame(f_out);

            if (bOnlyPoints) {
                clearImage(pframe);
                drawSquare(pframe, vMatchPairs[iFrameCounter], true);
                pframe->saveFrame(f_out2);
            }

            bfirstFrame = false;
        }

        pframeNext->saveFrame(f_out);

        if (bOnlyPoints) {
            pframeNextCopy->saveFrame(f_out2);
        }
        // memory manage
        delete pframe;
        delete pframeNextCopy;
        pframe = pframeNext;

       
        iFrameCounter++;

    }

    fclose(f_in);
    fclose(f_out);
    fclose(f_out2);

    delete pframe;

    return 0;
}
