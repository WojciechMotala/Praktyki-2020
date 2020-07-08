﻿#include <iostream>
#include <string>
#include "Frame.h"
#include "ezsift.h"

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

int main() {

    FILE* f_in;
    FILE* f_out;
    
    fopen_s(&f_in, "in.yuv", "rb");
    fopen_s(&f_out, "out.yuv", "wb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in %
    Frame* pframe = new Frame(1920, 1080, .5, .5);
    Frame* pframeNext;
    
    bool bfirstFrame = true;

    pframe->getFrame(f_in);
   
        while (!feof(f_in)) {
            
            pframeNext = new Frame(1920, 1080, 0.5, 0.5);
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

            imageFirst.read_pgm_direct(pframeBufY, iframeWidth, iframeHeight, iframeStrideWidthY);
            imageSecond.read_pgm_direct(pframeNextBufY, iframeNextWidth, iframeNextHeight, iframeNextStrideWidthY);

            // Detect keypoints
            list<ezsift::SiftKeypoint> kpt_list_first;
            list<ezsift::SiftKeypoint> kpt_list_second;
            bool bExtractDescriptor = true;

            sift_cpu(imageFirst, kpt_list_first, bExtractDescriptor);
            sift_cpu(imageSecond, kpt_list_second, bExtractDescriptor);

            // Match keypoints.
            list<ezsift::MatchPair> match_list;
            match_keypoints(kpt_list_first, kpt_list_second, match_list);

            // X Y move compensation 
            int* moveXY = moveCompensation(match_list);
            pframeNext->correctFramePosition(moveXY[0], moveXY[1]);
            
            // save frame to output file
            if (bfirstFrame) {
                pframe->saveFrame(f_out);
                bfirstFrame = false;
            }
            pframeNext->saveFrame(f_out);

            // memory manage
            delete pframe;
            pframe = pframeNext;
        }
    
    fclose(f_in);
    fclose(f_out);
    
    delete pframe;

    return 0;
}
