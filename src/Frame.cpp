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

void Frame::harrisCornerDetector() {

}

uint8_t* Frame::getIx() {
	uint8_t* Ix = new uint8_t[iStrideWidthY * iStrideHeightY];
	
	uint8_t* tempBufY = bufY;
	uint8_t* tempIx = &Ix[shiftY];

	
	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			int maskValue = tempBufY[j - 1 - iStrideWidthY] + 2 * tempBufY[j - iStrideWidthY] + tempBufY[j + 1 - iStrideWidthY] - tempBufY[j - 1 + iStrideWidthY] - 2 * tempBufY[j + iStrideWidthY] - tempBufY[j + 1 + iStrideWidthY];
			if (maskValue < 0) {
				tempIx[j] = 1;
			}
			else {
				tempIx[j] = maskValue;
			}
		
		}
		tempBufY += iStrideWidthY;
		tempIx += iStrideWidthY;
	}
	
	//************************************************************
	tempIx = &Ix[shiftY];
	FILE* test;
	
	fopen_s(&test, "stepsResult/testPoziomSobel.yuv", "wb");

	for (int i = 0; i < iHeightY; i++) {
		fwrite(tempIx + i * iStrideWidthY, 1, iWidthY, test);
	}
	for (int i = 0; i < iHeightU; i++) {
		fwrite(bufU + i * iStrideWidthU, 1, iWidthU, test);
	}
	for (int i = 0; i < iHeightV; i++) {
		fwrite(bufV + i * iStrideWidthV, 1, iWidthV, test);
	}
	
	fclose(test);
	//************************************************************

	return Ix;
}

uint8_t* Frame::getIy() {

	uint8_t* Iy = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempBufY = bufY;
	uint8_t* tempIy = &Iy[shiftY];


	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			int maskValue = -1 * tempBufY[j - 1 - iStrideWidthY] - 2 * tempBufY[j - 1] - tempBufY[j + iStrideWidthY - 1] + tempBufY[j + 1 - iStrideWidthY] + 2 * tempBufY[j + 1] + tempBufY[j + 1 + iStrideWidthY];
			if (maskValue < 0) {
				tempIy[j] = 1;
			}
			else {
				tempIy[j] = maskValue;
			}
		}
		tempBufY += iStrideWidthY;
		tempIy += iStrideWidthY;
	}

	//************************************************************
	tempIy = &Iy[shiftY];
	FILE* test;

	fopen_s(&test, "stepsResult/testPionSobel.yuv", "wb");

	for (int i = 0; i < iHeightY; i++) {
		fwrite(tempIy + i * iStrideWidthY, 1, iWidthY, test);
	}
	for (int i = 0; i < iHeightU; i++) {
		fwrite(bufU + i * iStrideWidthU, 1, iWidthU, test);
	}
	for (int i = 0; i < iHeightV; i++) {
		fwrite(bufV + i * iStrideWidthV, 1, iWidthV, test);
	}

	fclose(test);
	//************************************************************

	return Iy;
}

void Frame::squareI(uint8_t* I) {

	uint8_t* tempI = &I[shiftY];

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempI[j] = tempI[j] * tempI[j];
		}
		tempI += iStrideWidthY;
	}

}

uint8_t* Frame::multiplyIxIy(uint8_t* Ix, uint8_t* Iy)
{
	uint8_t* IxIy = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempIx = &Ix[shiftY];
	uint8_t* tempIy = &Iy[shiftY];
	uint8_t* tempIxIy = &IxIy[shiftY];
	
	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempIxIy[j] = tempIx[j] * tempIy[j];
		}
		tempIx += iStrideWidthY;
		tempIy += iStrideWidthY;
		tempIxIy += iStrideWidthY;
	}

	//************************************************************
	tempIxIy = &IxIy[shiftY];
	FILE* test;

	fopen_s(&test, "stepsResult/testIxIy.yuv", "wb");

	for (int i = 0; i < iHeightY; i++) {
		fwrite(tempIxIy + i * iStrideWidthY, 1, iWidthY, test);
	}
	for (int i = 0; i < iHeightU; i++) {
		fwrite(bufU + i * iStrideWidthU, 1, iWidthU, test);
	}
	for (int i = 0; i < iHeightV; i++) {
		fwrite(bufV + i * iStrideWidthV, 1, iWidthV, test);
	}

	fclose(test);
	//************************************************************
	
	return IxIy;
}

uint8_t* Frame::gauss(uint8_t* I) {

	uint8_t* gI = new uint8_t[iStrideWidthY * iStrideHeightY];
	
	uint8_t* tempgI = &gI[shiftY];
	uint8_t* tempI = &I[shiftY];

	// filtr:
	// 1/16, 2/16, 1/16
	// 2/16, 4/16, 2/16
	// 1/16, 2/16, 1/16

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
				//tempgI[j] = ( tempI[j - 1 - iStrideWidthY] + 2* tempI[j - iStrideWidthY] + tempI[j + 1 - iStrideWidthY] + 2* tempI[j - 1] + 4* tempI[j] + 2* tempI[j + 1] + tempI[j - 1 + iStrideWidthY] + 2* tempI[j + iStrideWidthY] + tempI[j + 1 + iStrideWidthY] ) / 16;
				tempgI[j] = (tempI[j - 1 - iStrideWidthY] + tempI[j - iStrideWidthY] + tempI[j + 1 - iStrideWidthY] + tempI[j - 1] + tempI[j] + tempI[j + 1] + tempI[j - 1 + iStrideWidthY] + tempI[j + iStrideWidthY] + tempI[j + 1 + iStrideWidthY]) / 9;
		}
		tempgI += iStrideWidthY;
		tempI += iStrideWidthY;
	}

	//************************************************************
	static int no = 0;
	tempgI = &gI[shiftY];
	FILE* test;

	switch (no)
	{
	case 0:
		fopen_s(&test, "stepsResult/testGaussIx.yuv", "wb");
		break;
	case 1:
		fopen_s(&test, "stepsResult/testGaussIy.yuv", "wb");
		break;
	case 2:
		fopen_s(&test, "stepsResult/testGaussIxIy.yuv", "wb");
		break;
	default:
		fopen_s(&test, "stepsResult/testGauss.yuv", "wb");
		break;
	}
	no++;

	for (int i = 0; i < iHeightY; i++) {
		fwrite(tempgI + i * iStrideWidthY, 1, iWidthY, test);
	}
	for (int i = 0; i < iHeightU; i++) {
		fwrite(bufU + i * iStrideWidthU, 1, iWidthU, test);
	}
	for (int i = 0; i < iHeightV; i++) {
		fwrite(bufV + i * iStrideWidthV, 1, iWidthV, test);
	}

	fclose(test);
	//************************************************************
	
	return gI;
}

uint8_t* Frame::harris(uint8_t* gIx, uint8_t* gIy, uint8_t* gIxIy)
{
	uint8_t* r = new uint8_t[iStrideWidthY * iStrideHeightY];
	
	uint8_t* tempr = &r[shiftY];
	uint8_t* Ix2 = &gIx[shiftY];
	uint8_t* Iy2 = &gIy[shiftY];
	uint8_t* IxIy = &gIxIy[shiftY];
	int k = 0.04; //0.04-0.06

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempr[j] = (Ix2[j] * Iy2[j] - (IxIy[j] * IxIy[j])) - k * ((Ix2[j] + Iy2[j]) * (Ix2[j] + Iy2[j]));
		}
		tempr += iStrideWidthY;
		Ix2 += iStrideWidthY;
		Iy2 += iStrideWidthY;
		IxIy += iStrideWidthY;
	}

	return r;
}

void Frame::checkCorners(uint8_t* harris) {

	uint8_t* tempHarris = &harris[shiftY];

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			if (tempHarris[j] > 254) {
				drawSquare(i, j);
			}
		}
		tempHarris += iStrideWidthY;
	}
	
}

void Frame::drawSquare(int height, int width) {
	uint8_t* tempBufY = bufY;

	int position = width + (height * iStrideWidthY);

	for (int k = position - 2; k <= position + 2; k++) {
		//horizontal lines
		tempBufY[k - 2 * iStrideWidthY] = 0;
		tempBufY[k + 2 * iStrideWidthY] = 0;
	}

	tempBufY[position - 2] = 0;
	tempBufY[position + 2] = 0;
	tempBufY[position - 2 - iStrideWidthY] = 0;
	tempBufY[position - 2 + iStrideWidthY] = 0;
	tempBufY[position + 2 - iStrideWidthY] = 0;
	tempBufY[position + 2 + iStrideWidthY] = 0;

}

void Frame::drawSquare(std::list<ezsift::SiftKeypoint> kpt_list){

	std::list<ezsift::SiftKeypoint>::iterator it;

	for (it = kpt_list.begin(); it != kpt_list.end(); it++) {

		int iRow = (int)it->r;
		int iCol = (int)it->c;

		drawSquare(iRow, iCol);
	}

}

void Frame::drawSquare(std::list<ezsift::MatchPair> match_list, bool firstFrame) {

	std::list<ezsift::MatchPair>::iterator pair;

	for (pair = match_list.begin(); pair != match_list.end(); pair++) {

		if (firstFrame) {
			int iRow = (int)pair->r1;
			int iCol = (int)pair->c1;

			drawSquare(iRow, iCol);
		}
		else {
			int iRow = (int)pair->r2;
			int iCol = (int)pair->c2;

			drawSquare(iRow, iCol);
		}

	}
}


void Frame::saveTopgm()
{
	//************************************************************
	uint8_t* tempY = bufY;
	FILE* test;

	fopen_s(&test, "stepsResult/TESTpgm.pgm", "wb");

	//pgm format 
	const char* tekst = "P5\n1920 1080\n255\n";
	fwrite(tekst,sizeof(char), strlen(tekst),test);
	
	for (int i = 0; i < iHeightY; i++) {	
		fwrite(tempY, 1, iWidthY, test);
		tempY += iStrideWidthY;
	}

	fclose(test);
	//************************************************************

}

int Frame::getStrideWidthY() {
	return iStrideWidthY;
}

int Frame::getWidthY() {
	return iWidthY;
}

int Frame::getHeightY() {
	return iHeightY;
}

void Frame::correctFramePosition(int moveX, int moveY) {
	
	uint8_t* oldBufY = bufY;
	uint8_t* oldBufU = bufU;
	uint8_t* oldBufV = bufV;
	
	// temp buffors to copy data
	uint8_t* const tempBufY = new uint8_t[iWidthY * iHeightY];
	uint8_t* const tempBufU = new uint8_t[iWidthU * iHeightU];
	uint8_t* const tempBufV = new uint8_t[iWidthV * iHeightV];

	// pointers to temp buffors
	uint8_t* ptempBufY = tempBufY;
	uint8_t* ptempBufU = tempBufU;
	uint8_t* ptempBufV = tempBufV;

	// copy data Y U V to temp buffors
	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			ptempBufY[j] = oldBufY[j]; 
			oldBufY[j] = 0;
		}
		ptempBufY += iWidthY;
		oldBufY += iStrideWidthY;
	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {
			ptempBufU[j] = oldBufU[j]; 
			oldBufU[j] = 0;
		}
		ptempBufU += iWidthU;
		oldBufU += iStrideWidthU;
	}

	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {
			ptempBufV[j] = oldBufV[j]; 
			oldBufV[j] = 0;
		}
		ptempBufV += iWidthV;
		oldBufV += iStrideWidthV;
	}

	// set pointers on corrected X Y positions
	// - 
	oldBufY = bufY;
	oldBufY += moveX + moveY * iStrideWidthY;

	oldBufU = bufU;
	oldBufU += moveX/2 + moveY/2 * iStrideWidthU;

	oldBufV = bufV;
	oldBufV += moveX/2 + moveY/2 * iStrideWidthV;

	ptempBufY = tempBufY;
	ptempBufU = tempBufU;
	ptempBufV = tempBufV;

	// reload Y U V data with corected X Y move
	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			oldBufY[j] = ptempBufY[j];
		}
		ptempBufY += iWidthY;
		oldBufY += iStrideWidthY;
	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {
			oldBufU[j] = ptempBufU[j];
		}
		ptempBufU += iWidthU;
		oldBufU += iStrideWidthU;
	}
	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {
			oldBufV[j] = ptempBufV[j];
		}
		ptempBufV += iWidthV;
		oldBufV += iStrideWidthV;
	}

	// delete temp buffors
	delete[] tempBufY;
	delete[] tempBufU;
	delete[] tempBufV;

}

void Frame::correctFrameRotation(float rotMat[]) {

	uint8_t* oldBufY = bufY;
	uint8_t* oldBufU = bufU;
	uint8_t* oldBufV = bufV;

	// temp buffors to copy data
	uint8_t* const tempBufY = new uint8_t[iWidthY * iHeightY];
	uint8_t* const tempBufU = new uint8_t[iWidthU * iHeightU];
	uint8_t* const tempBufV = new uint8_t[iWidthV * iHeightV];

	// pointers to temp buffors
	uint8_t* ptempBufY = tempBufY;
	uint8_t* ptempBufU = tempBufU;
	uint8_t* ptempBufV = tempBufV;

	// copy data Y U V to temp buffors
	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			ptempBufY[j] = oldBufY[j];
			oldBufY[j] = 0;
		}
		ptempBufY += iWidthY;
		oldBufY += iStrideWidthY;
	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {
			ptempBufU[j] = oldBufU[j];
			//oldBufU[j] = 0;
		}
		ptempBufU += iWidthU;
		oldBufU += iStrideWidthU;
	}

	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {
			ptempBufV[j] = oldBufV[j];
			//oldBufV[j] = 0;
		}
		ptempBufV += iWidthV;
		oldBufV += iStrideWidthV;
	}

	// set pointers on start positions
	oldBufY = bufY;
	oldBufU = bufU;
	oldBufV = bufV;

	ptempBufY = tempBufY;
	ptempBufU = tempBufU;
	ptempBufV = tempBufV;

	//theta = (theta * 3.14159265359) / 180;

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
															//-
			int iNewX = round( rotMat[0] * (j - iWidthY / 2) + rotMat[1] * (i - iHeightY / 2) + iWidthY / 2);
			int iNewY = round( rotMat[2] * (j - iWidthY / 2) + rotMat[3] * (i - iHeightY / 2) + iHeightY / 2);
			
			oldBufY[iNewX + (iNewY-i) * iStrideWidthY] = ptempBufY[j];

		}
		ptempBufY += iWidthY;
		oldBufY += iStrideWidthY;
		
	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {


			int iNewX = round(rotMat[0] * (j - iWidthU / 2) + rotMat[1] * (i - iHeightU / 2) + iWidthU / 2);
			int iNewY = round(rotMat[2] * (j - iWidthU / 2) + rotMat[3] * (i - iHeightU / 2) + iHeightU / 2);

			oldBufU[iNewX + (iNewY - i) * iStrideWidthU] = ptempBufU[j];
		}
		ptempBufU += iWidthU;
		oldBufU += iStrideWidthU;

	}

	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {

			
			int iNewX = round(rotMat[0] * (j - iWidthV / 2) + rotMat[1] * (i - iHeightV / 2) + iWidthV / 2);
			int iNewY = round(rotMat[2] * (j - iWidthV / 2) + rotMat[3] * (i - iHeightV / 2) + iHeightV / 2);

			oldBufV[iNewX + (iNewY - i) * iStrideWidthV] = ptempBufV[j];
		}
		ptempBufV += iWidthV;
		oldBufV += iStrideWidthV;

	}
	
	// delete temp buffors
	delete[] tempBufY;
	delete[] tempBufU;
	delete[] tempBufV;

}

uint8_t* Frame::getBufY() const
{
	return bufY;
}

void Frame::filtration() {

	uint8_t* tempBufY = bufY;

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempBufY[j] = tempBufY[j - 1 - iStrideWidthY] / 9 +
				tempBufY[j - iStrideWidthY] / 9 +
				tempBufY[j + 1 - iStrideWidthY] / 9 +
				tempBufY[j - 1] / 9 +
				tempBufY[j] / 9 +
				tempBufY[j + 1] / 9 +
				tempBufY[j - 1 + iStrideWidthY] / 9 +
				tempBufY[j + iStrideWidthY] / 9 +
				tempBufY[j + 1 + iStrideWidthY] / 9;
		}
		tempBufY += iStrideWidthY;
	}

}

void Frame::clearImage() {

	uint8_t* tempBufY = bufY;
	uint8_t* tempBufU = bufU;
	uint8_t* tempBufV = bufV;

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempBufY[j] = 255;
		}
		tempBufY += iStrideWidthY;
	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {
			tempBufU[j] = 128;
		}
		tempBufU += iStrideWidthU;
	}

	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {
			tempBufV[j] = 128;
		}
		tempBufV += iStrideWidthV;
	}

}

