#include <iostream>

using namespace std;

void changeLuminanceOfPixel(int8_t* buffer, int row, int col, int value) {
    int position = row * 1920 + col;
    buffer[position] = value;
}


int main() {

    FILE* f_in;
    FILE* f_out;
    int8_t* buffer;

    int frameSizeInBytes = 3110400;
    int countOfY = 2073600;
    int countOfU = 51840;
    int countOfV = 51840;
    
    fopen_s(&f_in, "in.yuv", "rb");
    fopen_s(&f_out, "out.yuv", "wb");

    // allocate memory to contain the whole frame:
    buffer = (int8_t*)malloc(frameSizeInBytes);

    while (!feof(f_in)) {
        
        //load one frame YUV
        fread(buffer, 1, frameSizeInBytes, f_in);
        
        //======================================================
        // Operations at one frame 
        //======================================================
        for (int j = 10; j < 1920; j += 10) {
            for (int i = 0; i < 1080; i++) {
                changeLuminanceOfPixel(buffer, i, j, 0);
            }
        }
        
        for (int i = 0; i < 1920; i++) {
            changeLuminanceOfPixel(buffer, 10, i, 0);
        }
        
        //======================================================

        //frite single frame to output file
        fwrite(buffer, 1, frameSizeInBytes, f_out);

    }

    fclose(f_in);
    fclose(f_out);

    free(buffer);

    return 0;
}