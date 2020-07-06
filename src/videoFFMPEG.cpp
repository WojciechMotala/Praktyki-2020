#include <iostream>
#include "Frame.h"

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

        uint8_t* Ix = frame->getIx();
        uint8_t* Iy = frame->getIy();

        frame->squareI(Ix);
        frame->squareI(Iy);

        uint8_t* IxIy = frame->multiplyIxIy(Ix, Iy);

        uint8_t* gaussIx = frame->gaussConvolve(Ix);
        uint8_t* gaussIy = frame->gaussConvolve(Iy);
        uint8_t* gaussIxIy = frame->gaussConvolve(IxIy);

        frame->saveFrame(f_out);

        uint8_t* harris = frame->harris(gaussIx, gaussIy, gaussIxIy);

        delete Ix;
        delete Iy;
        delete IxIy;
        delete gaussIx;
        delete gaussIy;
        delete gaussIxIy;
    //}

    fclose(f_in);
    fclose(f_out);

    delete frame;

    return 0;
}