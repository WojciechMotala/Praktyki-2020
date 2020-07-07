#include <iostream>
#include "Frame.h"
#include "ezsift.h"

using namespace std;

int main() {

    FILE* f_in;
    FILE* f_out;
    
    fopen_s(&f_in, "in.yuv", "rb");
    fopen_s(&f_out, "out.yuv", "wb");

    // args: imageWidth, imageHeight, StrideWidth in %, StrideHeight in % 
    Frame* frame = new Frame(1920, 1080, 10, 10);

    //while (!feof(f_in)) {
        
        frame->getFrame(f_in);
        frame->saveTopgm();
        // wyznaczenie linii 
        uint8_t* Ix = frame->getIx();
        uint8_t* Iy = frame->getIy();

        frame->squareI(Ix);
        frame->squareI(Iy);

        uint8_t* IxIy = frame->multiplyIxIy(Ix, Iy);

        uint8_t* gaussIx = frame->gauss(Ix);
        uint8_t* gaussIy = frame->gauss(Iy);
        uint8_t* gaussIxIy = frame->gauss(IxIy);

        uint8_t* harris = frame->harris(gaussIx, gaussIy, gaussIxIy);
        
        frame->checkCorners(harris);
        
        frame->saveFrame(f_out);

        delete Ix;
        delete Iy;
        delete IxIy;
        delete gaussIx;
        delete gaussIy;
        delete gaussIxIy;
        delete harris;
   // }

    fclose(f_in);
    fclose(f_out);

    delete frame;
    
    //************************************
    //ezSIFT:

    ezsift::Image<uint8_t> image1, image2;
    image1.read_pgm("test1.pgm");
    image2.read_pgm("test2.pgm");

    // Detect keypoints
    list<ezsift::SiftKeypoint> kpt_list1, kpt_list2;
    bool bExtractDescriptor = true;
    sift_cpu(image1, kpt_list1, bExtractDescriptor);
    sift_cpu(image2, kpt_list2, bExtractDescriptor);

    draw_keypoints_to_ppm_file("output1.ppm", image1, kpt_list1);
    draw_keypoints_to_ppm_file("output2.ppm", image2, kpt_list2);
    // Match keypoints.
    list<ezsift::MatchPair> match_list;
    match_keypoints(kpt_list1, kpt_list2, match_list);

    // Draw result image.
    draw_match_lines_to_ppm_file("output_1match2.ppm", image1, image2, match_list);
    printf("Number of matched keypoints: %d\n", match_list.size());

    //************************************

    return 0;
}