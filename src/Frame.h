#pragma once
#include <iostream>
#include <Eigen/Dense>
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
	Frame() {};
	Frame(int imageWidth, int imageHeight, float strideWidth, float strideHeight);
	void FrameCopy(const Frame& source);
	~Frame();

	void getFrame(FILE* inputFile);
	void saveFrame(FILE* outputFile);

	int getStrideWidthY();
	int getStrideHeightY();
	int getWidthY();
	int getHeightY();

	int getStrideWidthU();
	int getStrideHeightU();
	int getWidthU();
	int getHeightU();

	int getStrideWidthV();
	int getStrideHeightV();
	int getWidthV();
	int getHeightV();

	int getShiftY();

	uint8_t* getBufY () const;
	uint8_t* getBufU() const;
	uint8_t* getBufV() const;

};