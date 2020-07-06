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

        frame->colorCorners(harris);

        frame->saveFrame(f_out);

        delete Ix;
        delete Iy;
        delete IxIy;
        delete gaussIx;
        delete gaussIy;
        delete gaussIxIy;
        delete harris;
    //}

    fclose(f_in);
    fclose(f_out);

    delete frame;

    return 0;
}