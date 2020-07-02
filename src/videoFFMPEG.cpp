#include <iostream>

using namespace std;

int main()
{

    FILE* f_in;
    FILE* f_out;
    long fSize;
    char* buffer;
    size_t result;

    fopen_s(&f_in, "in.yuv","rb");

    // obtain file size:
    fseek(f_in, 0, SEEK_END);
    fSize = ftell(f_in);
    rewind(f_in);

    // allocate memory to contain the whole file:
    buffer = (char*)malloc(sizeof(char) * fSize);
    result = fread(buffer, 1, fSize, f_in);


    // access to single pixel
    for (int i = 0; i < 100000; i+=3) {
        buffer[i] = 120;
        buffer[i+1] = 120;
        buffer[i+2] = 120;
    }
        


    fclose(f_in);

    fopen_s(&f_out, "out.yuv", "wb");
    fwrite(buffer, sizeof(buffer[1]), fSize, f_out);
    fclose(f_out);
    free(buffer);

    return 0;
}


