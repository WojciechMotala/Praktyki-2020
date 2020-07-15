#include "Frame.h"

Frame::Frame(int imageWidth, int imageHeight, float strideWidth, float strideHeight) {
	
	this->iWidthY = imageWidth;
	this->iHeightY = imageHeight;

	this->iWidthU = imageWidth / 2;
	this->iHeightU = imageHeight / 2;

	this->iWidthV = imageWidth / 2;
	this->iHeightV = imageHeight / 2;

	// size of buffors
	if (strideWidth != 0 and strideHeight != 0) {
		this->iStrideWidthY = iWidthY + iWidthY * strideWidth;
		this->iStrideWidthU = iWidthU + iWidthV * strideWidth;
		this->iStrideWidthV = iWidthV + iWidthV * strideWidth;

		this->iStrideHeightY = iHeightY + iHeightY * strideHeight;
		this->iStrideHeightU = iHeightU + iHeightU * strideHeight;
		this->iStrideHeightV = iHeightV + iHeightV * strideHeight;
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

void Frame::FrameCopy(const Frame& source) {
	this->iWidthY = source.iWidthY;
	this->iHeightY = source.iHeightY;

	this->iWidthU = source.iWidthU;
	this->iHeightU = source.iHeightU;

	this->iWidthV = source.iWidthV;
	this->iHeightV = source.iHeightV;

	// size of buffors

	this->iStrideWidthY = source.iStrideWidthY;
	this->iStrideWidthU = source.iStrideWidthU;
	this->iStrideWidthV = source.iStrideWidthV;

	this->iStrideHeightY = source.iStrideHeightY;
	this->iStrideHeightU = source.iStrideHeightU;
	this->iStrideHeightV = source.iStrideHeightV;

	// length of data
	this->iTotalLengthY = source.iTotalLengthY;
	this->iTotalLengthU = source.iTotalLengthU;
	this->iTotalLengthV = source.iTotalLengthV;

	this->orgBufY = new uint8_t[iStrideWidthY * iStrideHeightY];
	this->orgBufU = new uint8_t[iStrideWidthU * iStrideHeightU];
	this->orgBufV = new uint8_t[iStrideWidthV * iStrideHeightV];

	this->shiftY = source.shiftY;
	this->shiftU = source.shiftU;
	this->shiftV = source.shiftV;

	this->bufY = &orgBufY[shiftY];
	this->bufU = &orgBufU[shiftU];
	this->bufV = &orgBufV[shiftV];


	uint8_t* tmpBufY = bufY;
	uint8_t* tmpBufU = bufU;
	uint8_t* tmpBufV = bufV;

	uint8_t* tmpSourceBufY = source.bufY;
	uint8_t* tmpSourceBufU = source.bufU;
	uint8_t* tmpSourceBufV = source.bufV;

	//copy bufors data
	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tmpBufY[j] = tmpSourceBufY[j];
		}
		tmpBufY += iStrideWidthY;
		tmpSourceBufY += iStrideWidthY;
	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {
			tmpBufU[j] = tmpSourceBufU[j];
		}
		tmpBufU += iStrideWidthU;
		tmpSourceBufU += iStrideWidthU;
	}

	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {
			tmpBufV[j] = tmpSourceBufV[j];
		}
		tmpBufV += iStrideWidthV;
		tmpSourceBufV += iStrideWidthV;
	}

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

int Frame::getStrideWidthY() {
	return iStrideWidthY;
}

int Frame::getStrideHeightY()
{
	return iStrideHeightY;
}

int Frame::getWidthY() {
	return iWidthY;
}

int Frame::getHeightY() {
	return iHeightY;
}

int Frame::getStrideWidthU() {
	return iStrideWidthU;
}

int Frame::getStrideHeightU() {
	return iStrideHeightU;
}

int Frame::getWidthU() {
	return iWidthU;
}

int Frame::getHeightU() {
	return iHeightU;
}

int Frame::getStrideWidthV() {
	return iStrideWidthV;
}

int Frame::getStrideHeightV() {
	return iStrideHeightV;
}

int Frame::getWidthV() {
	return iWidthV;
}

int Frame::getHeightV() {
	return iHeightV;
}

int Frame::getShiftY() {
	return shiftY;
}

uint8_t* Frame::getBufY() const {
	return bufY;
}

uint8_t* Frame::getBufU() const {
	return bufU;
}

uint8_t* Frame::getBufV() const {
	return bufV;
}
