#include <iostream>
#include <Eigen/Dense>
#include <math.h>
#include <vector>
#include <array>
#include <fstream>
#include "Frame.h"
#include "ezsift.h"


#define PI 3.14159265359

using namespace std;
using namespace Eigen;

int* moveCompensation(list<ezsift::MatchPair> match_list) {

    double meanX = 0;
    double meanY = 0;

    list<ezsift::MatchPair>::iterator pair;
    int counterX = 0;
    int counterY = 0;
    
    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;
        
        if ((pair->c1 - pair->c2) != 0) {
            meanX += pair->c1 - pair->c2;
            counterX++;
        }
        
        if ((pair->r1 - pair->r2) != 0) {
            meanY += pair->r1 - pair->r2;
            counterY++;
        }
        
    }

    meanX /= counterX;
    meanY /= counterY;

    int resX = round(meanX);
    int resY = round(meanY);

    int iCompensationXY[2];
    iCompensationXY[0] = resX;
    iCompensationXY[1] = resY;

    return iCompensationXY;

}

float calculateRotationAngle(list<ezsift::MatchPair> match_list) {

    list<ezsift::MatchPair>::iterator pair;

    float fAngle = 0.0;
    int iCounter = 0;

    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;

        /*
        // slope of a straight line
        float fA1 = (float)pair->r1 / (float)pair->c1;
        float fA2 = (float)pair->r2 / (float)pair->c2;

        float fThetaA1 = atan(fA1);
        float fThetaA2 = atan(fA2);
        */

        //-----
        float Theta1 = atan2((float)pair->r1, (float)pair->c1);
        float Theta2 = atan2((float)pair->r2, (float)pair->c2);
        float fTheta = Theta1 - Theta2;
        //-----

        //float fTheta = fThetaA1 - fThetaA2;

        fAngle += fTheta;
        iCounter++;
    }

    if(iCounter != 0)
        fAngle /= iCounter;

    return fAngle;
}

float calculateCenterRotationAngle(list<ezsift::MatchPair> match_list, int width, int height) {

    list<ezsift::MatchPair>::iterator pair;

    float fAngle = 0.0;
    int iCounter = 0;

    int iCenterX = width / 2;
    int iCenterY = height / 2;

    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;

        // slope of a straight line
        float fA1 = ((float)iCenterY - (float)pair->r1) / ((float)iCenterX - (float)pair->c1);
        float fA2 = ((float)iCenterY - (float)pair->r2) / ((float)iCenterX - (float)pair->c2);

        float fThetaA1 = atan(fA1);
        float fThetaA2 = atan(fA2);

        float fTheta = fThetaA1 - fThetaA2;

        fAngle += fTheta;
        iCounter++;
    }

    fAngle /= iCounter;

    return fAngle;
}

void mulMat(float rotMat[], float theta) {
    float mat[4] = { cos(theta), -sin(theta),
                     sin(theta),  cos(theta) };

    float tempMat[4] = { rotMat[0], rotMat[1],
                         rotMat[2], rotMat[3] };


    rotMat[0] = tempMat[0] * mat[0] + tempMat[1] * mat[2];
    rotMat[1] = tempMat[0] * mat[1] + tempMat[1] * mat[3];
    rotMat[2] = tempMat[2] * mat[0] + tempMat[3] * mat[2];
    rotMat[3] = tempMat[2] * mat[1] + tempMat[3] * mat[3];

}

Matrix3f calcHmat(list<ezsift::MatchPair> match_list) {
    
    list<ezsift::MatchPair>::iterator pair;

    int iM = 3;
    int iN = match_list.size();

    MatrixXf M1(iM, iN);
    MatrixXf M2(iM, iN);
    
    int iCol;

    // populate matrixes
    for (iCol = 0, pair = match_list.begin(); pair != match_list.end(); pair++, iCol++) {
        
        M1(0, iCol) = pair->c2;
        M1(1, iCol) = pair->r2;
        M1(2, iCol) = 1;

        M2(0, iCol) = pair->c1;
        M2(1, iCol) = pair->r1;
        M2(2, iCol) = 1;
    }

    Matrix3f H = (M2 * M1.transpose()) * (M1 * M1.transpose()).inverse();

    //cout << H;
    return H;
}

int main() {

    FILE* f_in;
    FILE* f_out;
    
    //================================================================
    
    vector <array<float, 3>> vRTdata;
    vector <list<ezsift::MatchPair>> vMatchPairs;

    vector<Matrix3f> vHmatrix;

    array<float, 3> frameRT;

    // init for first frame
    Matrix3f firstFrameH;
    firstFrameH << 0, 0, 0, 0, 0, 0, 0, 0, 0;
    vHmatrix.push_back(firstFrameH);
    
    /*
    frameRT[0] = 0.0;
    frameRT[1] = 0.0;
    frameRT[2] = 0.0;

    vRTdata.push_back(frameRT);
    */
    //================================================================

    bool bRotation = false;
    bool bMakeRotation = false;
    bool bTranslation = false;
    bool bMakeTranslation = false;
    bool bDrawCharacteristicPoints = false;
    bool bOnlyCharacteristicPoints = false;

    fopen_s(&f_in, "in_srednie.yuv", "rb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in %
    Frame* pframe = new Frame(1920, 1080, 1.0, 1.0);
    Frame* pframeNext;

    Frame* pframeCopy = new Frame();
    Frame* pframeNextCopy;
    
    ofstream myfile;


    bool bfirstFrame = true;

    float fRotationAngle;
    int* moveXY = nullptr;
    //int test = 0;
    
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
            /*
            myfile.open("H.txt", std::ios::app);
            myfile << vHmatrix[test](0, 0) << "\n";
            myfile << vHmatrix[test](0, 1) << "\n";
            myfile << vHmatrix[test](0, 2) << "\n";
            myfile << vHmatrix[test](1, 0) << "\n";
            myfile << vHmatrix[test](1, 1) << "\n";
            myfile << vHmatrix[test](1, 2) << "\n";
            myfile << vHmatrix[test](2, 0) << "\n";
            myfile << vHmatrix[test](2, 1) << "\n";
            myfile << vHmatrix[test](2, 2) << "\n";
            test++;
            myfile.close();
            */
            // memory manage
            delete pframe;
            pframe = pframeNext;
            

            
        }
    
    fclose(f_in);

    delete pframe;


    // R and T compensation loop
    //vHmatrix.clear();
    fopen_s(&f_in, "in_srednie.yuv", "rb");
    fopen_s(&f_out, "out.yuv", "wb");
    /*
    fstream myInfile;
    myInfile.open("H.txt", ios_base::in);

    float a, b, c, d, e, f, g, h, i;
    while (myInfile >> a >> b >> c >> d >> e >> f >> g >> h >> i) {
        Matrix3f tmp;
        tmp << a, b, c, d, e, f, g, h, i;
        //cout << tmp;
        vHmatrix.push_back(tmp);
    }
    myInfile.close();
    */
    pframe = new Frame(1920, 1080, 1.0, 1.0);
    pframe->getFrame(f_in);

    if (bDrawCharacteristicPoints) {
        // copy of frame to mark characteristic points
        pframeCopy->FrameCopy(*pframe);
    }

    int iFrameCounter = 0;

    while (!feof(f_in)) {

        iFrameCounter++;

        pframeNext = new Frame(1920, 1080, 1.0, 1.0);
        pframeNextCopy = new Frame();

        pframeNext->getFrame(f_in);

        if (bDrawCharacteristicPoints) {
            // copy of frame to mark characteristic points
            pframeNextCopy->FrameCopy(*pframeNext);
        }

        if (feof(f_in))
            break;

        pframeNext->correctFrameByH(vHmatrix[iFrameCounter]);

        // save frame to output file
        if (bfirstFrame) {

            if (bDrawCharacteristicPoints) {
                pframeCopy->saveFrame(f_out);
                delete pframeCopy;
            }
            else {
                pframe->saveFrame(f_out);
            }

            bfirstFrame = false;
        }

        if (bDrawCharacteristicPoints) {
            pframeNextCopy->saveFrame(f_out);
            delete pframeNextCopy;
        }
        else {
            pframeNext->saveFrame(f_out);
        }

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
