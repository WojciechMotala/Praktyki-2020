#pragma once
#include <iostream>
#include "ezsift.h"

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
	Frame(int imageWidth, int imageHeight, float strideWidth, float strideHeight);
	~Frame();

	void getFrame(FILE* inputFile);
	void saveFrame(FILE* outputFile);
	void harrisCornerDetector();

	uint8_t* getIx();
	uint8_t* getIy();

	void squareI(uint8_t* I);
	uint8_t* multiplyIxIy(uint8_t* Ix, uint8_t* Iy);
	uint8_t* gauss(uint8_t* I);
	uint8_t* harris(uint8_t* gIx, uint8_t* gIy, uint8_t* gIxIy);

	void checkCorners(uint8_t* harris);
	void drawSquare(int height, int width);
	void drawSquare(std::list<ezsift::SiftKeypoint> kpt_list);
	void saveTopgm();

	int getStrideWidthY();
	int getWidthY();
	int getHeightY();

	void correctFramePosition(int moveX, int moveY);

	uint8_t* getBufY () const;

};