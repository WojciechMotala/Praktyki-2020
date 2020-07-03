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

    while (!feof(f_in)) {
        
        frame->getFrame(f_in);

        frame->saveFrame(f_out);

    }

    fclose(f_in);
    fclose(f_out);

    delete frame;

    return 0;
}