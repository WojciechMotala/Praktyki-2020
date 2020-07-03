#pragma once
#include <iostream>

class Frame {
private:

	int iWidthY;
	int iWidthU;
	int iWidthV;

	int iHeightY;
	int iHeightU;
	int iHeightV;

	int iStrideWidthY;
	int iStrideWidthU;
	int iStrideWidthV;

	int iStrideHeightY;
	int iStrideHeightU;
	int iStrideHeightV;
	
	int iTotalLengthY;
	int iTotalLengthU;
	int iTotalLengthV;

	uint8_t* orgBufY;
	uint8_t* orgBufU;
	uint8_t* orgBufV;

	uint8_t* bufY;
	uint8_t* bufU;
	uint8_t* bufV;

	int shiftY;
	int shiftU;
	int shiftV;


public:
	Frame(int imageWidth, int imageHeight, int strideWidth, int strideHeight);
	~Frame();

	void getFrame(FILE* inputFile);
	void saveFrame(FILE* outputFile);

};