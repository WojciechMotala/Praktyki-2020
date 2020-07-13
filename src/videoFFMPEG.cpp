﻿#include <iostream>
#include <math.h>
#include <vector>
#include <array>
#include <fstream>
#include "Frame.h"
#include "ezsift.h"

#define PI 3.14159265359

using namespace std;

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

void calcNewXY(float theta, int moveX, int moveY, list<ezsift::MatchPair> match_list) {

    list<ezsift::MatchPair>::iterator pair;

    static int frameCounter = 0;

    int oldX;
    int oldY;

    ofstream myfile;
    myfile.open("oldXY_newXY.txt", std::ios::app);
    myfile << "frame no.: " << frameCounter << "\n";

    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        oldX = pair->c1;
        oldY = pair->r1;

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;

        int iNewX = round(cos(theta) * (oldX - 1920 / 2) - sin(theta) * (oldY - 1080 / 2) + 1920 / 2);
        int iNewY = round(sin(theta) * (oldX - 1920 / 2) + cos(theta) * (oldY - 1080 / 2) + 1080 / 2);

        iNewX += moveX;
        iNewY += moveY;

        //=============================================
        
        myfile << "OldX: \t" << oldX << "\tOldY: \t" << oldY << "\tnewX: \t" << iNewX << "\tnewY: \t" << iNewY << "\n";

        //=============================================

        
    }
    frameCounter++;
    myfile.close();
}

int main() {

    FILE* f_in;
    FILE* f_out;
    
    //================================================================
    
    vector <array<float, 3>> vRTdata;

    array<float, 3> frameRT;

    // init for first frame
    frameRT[0] = 0.0;
    frameRT[1] = 0.0;
    frameRT[2] = 0.0;

    vRTdata.push_back(frameRT);

    //================================================================

    bool bRotation = true;
    bool bMakeRotation = true;
    bool bTranslation = true;
    bool bMakeTranslation = true;
    bool bDrawCharacteristicPoints = false;
    bool bOnlyCharacteristicPoints = false;

    fopen_s(&f_in, "in_srednie.yuv", "rb");
    fopen_s(&f_out, "out.yuv", "wb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in %
    Frame* pframe = new Frame(1920, 1080, 1.0, 1.0);
    Frame* pframeNext;

    Frame* pframeCopy = new Frame();
    Frame* pframeNextCopy;
    
    bool bfirstFrame = true;

    pframe->getFrame(f_in);

    if (bDrawCharacteristicPoints) {
        // copy of frame to mark characteristic points
        pframeCopy->FrameCopy(*pframe);
    }
    
    float fRotationAngle;
    int* moveXY = nullptr;

        while (!feof(f_in)) {
            

            pframeNext = new Frame(1920, 1080, 1.0, 1.0);
            pframeNextCopy = new Frame();

            pframeNext->getFrame(f_in);

            if (bDrawCharacteristicPoints) {
                // copy of frame to mark characteristic points
                pframeNextCopy->FrameCopy(*pframeNext);
            }

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

            if (bDrawCharacteristicPoints) {

                if (bfirstFrame && pframeCopy != nullptr) {
                    //pframeCopy->drawSquare(kpt_list_first);
                    if (bOnlyCharacteristicPoints)
                        pframeCopy->clearImage();

                    pframeCopy->drawSquare(match_list, true);
                }
                
                if (pframeNextCopy != nullptr) {
                    //pframeNextCopy->drawSquare(kpt_list_second);
                    if(bOnlyCharacteristicPoints)
                        pframeNextCopy->clearImage();
                    
                    pframeNextCopy->drawSquare(match_list, false);
                }
            }
            


            if (bRotation) {
                // rotation compensation
                fRotationAngle = calculateRotationAngle(match_list);

                if (abs(fRotationAngle) < 0.045) {
                    if (fRotationAngle != 0 && bMakeRotation) {
                        pframeNext->correctFrameRotation(fRotationAngle);

                        if (bDrawCharacteristicPoints)
                            pframeNextCopy->correctFrameRotation(fRotationAngle);
                    }
                }

                frameRT[0] = fRotationAngle;
            }

            if (bTranslation) {
                // translation compensation 
                moveXY = moveCompensation(match_list);
                if (moveXY != nullptr) {
                    frameRT[1] = moveXY[0];
                    frameRT[2] = moveXY[1];
                }
                if (bMakeTranslation) {
                    pframeNext->correctFramePosition(moveXY[0], moveXY[1]);

                    if (bDrawCharacteristicPoints)
                        pframeNextCopy->correctFramePosition(moveXY[0], moveXY[1]);
                }
            }


            calcNewXY(fRotationAngle, frameRT[1], frameRT[2], match_list);
            
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
                //save R and T data per frame
                vRTdata.push_back(frameRT);

                pframeNext->saveFrame(f_out);
            }
            

            // memory manage
            delete pframe;
            pframe = pframeNext;
            
            
        }
    
    fclose(f_in);
    fclose(f_out);
    
    delete pframe;






    /*
    //=============================================
    ofstream myfile;
    myfile.open("RTperFrame.txt", std::ios::app);

    for (int i = 0; i < vRTdata.size(); i++) {
        myfile << "frame no.: " << i << "\ttheta: " << vRTdata[i][0] << "\t\tX: " << vRTdata[i][1] << "\t\tY: " << vRTdata[i][2] << "\n";
    }

    myfile.close();
    //=============================================
    */

    return 0;
}
