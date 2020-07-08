#include <iostream>
#include <string>
#include "Frame.h"
#include "ezsift.h"

using namespace std;

int main() {

    FILE* f_in;
    FILE* f_out;
    
    fopen_s(&f_in, "in.yuv", "rb");
    fopen_s(&f_out, "out.yuv", "wb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in %
    Frame* pframe = new Frame(1920, 1080, 10, 10);
    Frame* pframeNext = new Frame(1920, 1080, 10, 10);

    while (!feof(f_in)) {
        
        pframe->getFrame(f_in);
        pframeNext->getFrame(f_in);

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
        
        // draw frame of keypoints on image
        pframe->drawSquare(kpt_list_first);
        pframeNext->drawSquare(kpt_list_second);

        // save frame to output file
        pframe->saveFrame(f_out);
        pframeNext->saveFrame(f_out);

    }
   
    fclose(f_in);
    fclose(f_out);
    
    delete pframe;
    delete pframeNext;
    
    return 0;
}