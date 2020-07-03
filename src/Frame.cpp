#include "Frame.h"

Frame::Frame(int imageWidth, int imageHeight, int strideWidth, int strideHeight) {
	
	this->iWidthY = imageWidth;
	this->iHeightY = imageHeight;

	this->iWidthU = imageWidth / 2;
	this->iHeightU = imageHeight / 2;

	this->iWidthV = imageWidth / 2;
	this->iHeightV = imageHeight / 2;

	// size of buffors
	if (strideWidth != 0 and strideHeight != 0) {
		this->iStrideWidthY = iWidthY + iWidthY / strideWidth;
		this->iStrideWidthU = iWidthU + iWidthV / strideWidth;
		this->iStrideWidthV = iWidthV + iWidthV / strideWidth;

		this->iStrideHeightY = iHeightY + iHeightY / strideHeight;
		this->iStrideHeightU = iHeightU + iHeightU / strideHeight;
		this->iStrideHeightV = iHeightV + iHeightV / strideHeight;
	}
	else {
		this->iStrideWidthY = iWidthY;
		this->iStrideWidthU = iWidthU;
		this->iStrideWidthV = iWidthV;

		this->iStrideHeightY = iHeightY;
		this->iStrideHeightU = iHeightU;
		this->iStrideHeightV = iHeightV;
	}


	// length of data
	this->iTotalLengthY = iWidthY * iHeightY;
	this->iTotalLengthU = iWidthU * iHeightU;
	this->iTotalLengthV = iWidthV * iHeightV;

	this->orgBufY = new uint8_t[iStrideWidthY * iStrideHeightY];
	this->orgBufU = new uint8_t[iStrideWidthU * iStrideHeightU];
	this->orgBufV = new uint8_t[iStrideWidthV * iStrideHeightV];

	this->shiftY = iStrideWidthY * ((iStrideHeightY - iHeightY) / 2) + ((iStrideWidthY - iWidthY) / 2);
	this->shiftU = iStrideWidthU * ((iStrideHeightU - iHeightU) / 2) + ((iStrideWidthU - iWidthU) / 2);
	this->shiftV = iStrideWidthV * ((iStrideHeightV - iHeightV) / 2) + ((iStrideWidthV - iWidthV) / 2);

	this->bufY = &orgBufY[shiftY];
	this->bufU = &orgBufU[shiftU];
	this->bufV = &orgBufV[shiftV];

}

Frame::~Frame() {
	delete[] orgBufY;
	delete[] orgBufU;
	delete[] orgBufV;
}

void Frame::getFrame(FILE* inputFile) {

	for (int i = 0; i < iHeightY; i++) {
		fread(bufY + i * iStrideWidthY, 1, iWidthY, inputFile);
	}
	for (int i = 0; i < iHeightU; i++) {
		fread(bufU + i * iStrideWidthU, 1, iWidthU, inputFile);
	}
	for (int i = 0; i < iHeightV; i++) {
		fread(bufV + i * iStrideWidthV, 1, iWidthV, inputFile);
	}

}

void Frame::saveFrame(FILE* outputFile) {

	for (int i = 0; i < iHeightY; i++) {
		fwrite(bufY + i * iStrideWidthY, 1, iWidthY, outputFile);
	}
	for (int i = 0; i < iHeightU; i++) {
		fwrite(bufU + i * iStrideWidthU, 1, iWidthU, outputFile);
	}
	for (int i = 0; i < iHeightV; i++) {
		fwrite(bufV + i * iStrideWidthV, 1, iWidthV, outputFile);
	}

}
