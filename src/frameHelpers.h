#pragma once
#include "Frame.h"

uint8_t* getIx(Frame* frame) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iStrideHeightY = frame->getStrideHeightY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iStrideHeightU = frame->getStrideHeightU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iStrideHeightV = frame->getStrideHeightV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* bufY = frame->getBufY();
	uint8_t* bufU = frame->getBufU();
	uint8_t* bufV = frame->getBufV();

	uint8_t* Ix = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempBufY = bufY;
	uint8_t* tempIx = &Ix[iShiftY];


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
	tempIx = &Ix[iShiftY];
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

uint8_t* getIy(Frame* frame) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iStrideHeightY = frame->getStrideHeightY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iStrideHeightU = frame->getStrideHeightU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iStrideHeightV = frame->getStrideHeightV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* bufY = frame->getBufY();
	uint8_t* bufU = frame->getBufU();
	uint8_t* bufV = frame->getBufV();

	uint8_t* Iy = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempBufY = bufY;
	uint8_t* tempIy = &Iy[iShiftY];


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
	tempIy = &Iy[iShiftY];
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

void squareI(Frame* frame, uint8_t* I) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	uint8_t* tempI = &I[iShiftY];

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempI[j] = tempI[j] * tempI[j];
		}
		tempI += iStrideWidthY;
	}

}

uint8_t* multiplyIxIy(Frame* frame, uint8_t* Ix, uint8_t* Iy)
{
	int iStrideWidthY = frame->getStrideWidthY();
	int iStrideHeightY = frame->getStrideHeightY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* bufU = frame->getBufU();
	uint8_t* bufV = frame->getBufV();

	uint8_t* IxIy = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempIx = &Ix[iShiftY];
	uint8_t* tempIy = &Iy[iShiftY];
	uint8_t* tempIxIy = &IxIy[iShiftY];

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			tempIxIy[j] = tempIx[j] * tempIy[j];
		}
		tempIx += iStrideWidthY;
		tempIy += iStrideWidthY;
		tempIxIy += iStrideWidthY;
	}

	//************************************************************
	tempIxIy = &IxIy[iShiftY];
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

uint8_t* gauss(Frame* frame, uint8_t* I) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iStrideHeightY = frame->getStrideHeightY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* bufU = frame->getBufU();
	uint8_t* bufV = frame->getBufV();

	uint8_t* gI = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempgI = &gI[iShiftY];
	uint8_t* tempI = &I[iShiftY];

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
	tempgI = &gI[iShiftY];
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

void saveTopgm(Frame* frame)
{

	int iStrideWidthY = frame->getStrideWidthY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	uint8_t* bufY = frame->getBufY();

	//************************************************************
	uint8_t* tempY = bufY;
	FILE* test;

	fopen_s(&test, "stepsResult/TESTpgm.pgm", "wb");

	//pgm format 
	const char* tekst = "P5\n1920 1080\n255\n";
	fwrite(tekst, sizeof(char), strlen(tekst), test);

	for (int i = 0; i < iHeightY; i++) {
		fwrite(tempY, 1, iWidthY, test);
		tempY += iStrideWidthY;
	}

	fclose(test);
	//************************************************************

}

uint8_t* harris(Frame* frame, uint8_t* gIx, uint8_t* gIy, uint8_t* gIxIy)
{
	int iStrideWidthY = frame->getStrideWidthY();
	int iStrideHeightY = frame->getStrideHeightY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	uint8_t* r = new uint8_t[iStrideWidthY * iStrideHeightY];

	uint8_t* tempr = &r[iShiftY];
	uint8_t* Ix2 = &gIx[iShiftY];
	uint8_t* Iy2 = &gIy[iShiftY];
	uint8_t* IxIy = &gIxIy[iShiftY];
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

void drawSquare(int height, int width, uint8_t* bufY, int iStrideWidthY) {
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

void checkCorners(Frame* frame, uint8_t* harris) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iShiftY = frame->getShiftY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	uint8_t* bufY = frame->getBufY();

	uint8_t* tempHarris = &harris[iShiftY];

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			if (tempHarris[j] > 254) {
				drawSquare(i, j, bufY, iStrideWidthY);
			}
		}
		tempHarris += iStrideWidthY;
	}

}

void drawSquare(Frame* frame, std::list<ezsift::SiftKeypoint> kpt_list) {

	std::list<ezsift::SiftKeypoint>::iterator it;

	uint8_t* bufY = frame->getBufY();
	int iStrideWidthY = frame->getStrideWidthY();

	for (it = kpt_list.begin(); it != kpt_list.end(); it++) {

		int iRow = (int)it->r;
		int iCol = (int)it->c;

		drawSquare(iRow, iCol, bufY, iStrideWidthY);
	}

}

void drawSquare(Frame* frame, std::list<ezsift::MatchPair> match_list, bool firstFrame) {

	std::list<ezsift::MatchPair>::iterator pair;

	uint8_t* bufY = frame->getBufY();
	int iStrideWidthY = frame->getStrideWidthY();

	for (pair = match_list.begin(); pair != match_list.end(); pair++) {

		if (firstFrame) {
			int iRow = (int)pair->r1;
			int iCol = (int)pair->c1;

			drawSquare(iRow, iCol, bufY, iStrideWidthY);
		}
		else {
			int iRow = (int)pair->r2;
			int iCol = (int)pair->c2;

			drawSquare(iRow, iCol, bufY, iStrideWidthY);
		}

	}
}

void correctFramePosition(Frame* frame, int moveX, int moveY) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* oldBufY = frame->getBufY();
	uint8_t* oldBufU = frame->getBufU();
	uint8_t* oldBufV = frame->getBufV();

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
	oldBufY = frame->getBufY();
	oldBufY += moveX + moveY * iStrideWidthY;

	oldBufU = frame->getBufU();
	oldBufU += moveX / 2 + moveY / 2 * iStrideWidthU;

	oldBufV = frame->getBufV();
	oldBufV += moveX / 2 + moveY / 2 * iStrideWidthV;

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

void correctFrameRotation(Frame* frame, float rotMat[]) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* oldBufY = frame->getBufY();
	uint8_t* oldBufU = frame->getBufU();
	uint8_t* oldBufV = frame->getBufV();

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
	oldBufY = frame->getBufY();
	oldBufU = frame->getBufU();
	oldBufV = frame->getBufV();

	ptempBufY = tempBufY;
	ptempBufU = tempBufU;
	ptempBufV = tempBufV;

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {
			//-
			int iNewX = round(rotMat[0] * (j - iWidthY / 2) + rotMat[1] * (i - iHeightY / 2) + iWidthY / 2);
			int iNewY = round(rotMat[2] * (j - iWidthY / 2) + rotMat[3] * (i - iHeightY / 2) + iHeightY / 2);

			oldBufY[iNewX + (iNewY - i) * iStrideWidthY] = ptempBufY[j];

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

void correctFrameByH(Frame* frame, Eigen::Matrix3f H) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* oldBufY = frame->getBufY();
	uint8_t* oldBufU = frame->getBufU();
	uint8_t* oldBufV = frame->getBufV();

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
	oldBufY = frame->getBufY();
	oldBufU = frame->getBufU();
	oldBufV = frame->getBufV();

	ptempBufY = tempBufY;
	ptempBufU = tempBufU;
	ptempBufV = tempBufV;

	for (int i = 0; i < iHeightY; i++) {
		for (int j = 0; j < iWidthY; j++) {

			Eigen::MatrixXf newXY(3, 1);
			Eigen::MatrixXf oldXY(3, 1);
			oldXY(0) = j;
			oldXY(1) = i;
			oldXY(2) = 1;

			newXY = H * oldXY;
			int iNewX = int(newXY(0));
			int iNewY = int(newXY(1));

			oldBufY[iNewX + (iNewY - i) * iStrideWidthY] = ptempBufY[j];

		}
		ptempBufY += iWidthY;
		oldBufY += iStrideWidthY;

	}

	for (int i = 0; i < iHeightU; i++) {
		for (int j = 0; j < iWidthU; j++) {

			Eigen::MatrixXf newXY(3, 1);
			Eigen::MatrixXf oldXY(3, 1);
			oldXY(0) = 2 * j;
			oldXY(1) = 2 * i;
			oldXY(2) = 1;

			newXY = H * oldXY;
			int iNewX = int(newXY(0)) / 2;
			int iNewY = int(newXY(1)) / 2;

			oldBufU[iNewX + (iNewY - i) * iStrideWidthU] = ptempBufU[j];

		}
		ptempBufU += iWidthU;
		oldBufU += iStrideWidthU;

	}

	for (int i = 0; i < iHeightV; i++) {
		for (int j = 0; j < iWidthV; j++) {

			Eigen::MatrixXf newXY(3, 1);
			Eigen::MatrixXf oldXY(3, 1);
			oldXY(0) = 2 * j;
			oldXY(1) = 2 * i;
			oldXY(2) = 1;

			newXY = H * oldXY;
			int iNewX = int(newXY(0)) / 2;
			int iNewY = int(newXY(1)) / 2;

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

void filtration(Frame* frame) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	uint8_t* tempBufY = frame->getBufY();

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

void clearImage(Frame* frame) {

	int iStrideWidthY = frame->getStrideWidthY();
	int iHeightY = frame->getHeightY();
	int iWidthY = frame->getWidthY();

	int iStrideWidthU = frame->getStrideWidthU();
	int iHeightU = frame->getHeightU();
	int iWidthU = frame->getWidthU();

	int iStrideWidthV = frame->getStrideWidthV();
	int iHeightV = frame->getHeightV();
	int iWidthV = frame->getWidthV();

	uint8_t* tempBufY = frame->getBufY();
	uint8_t* tempBufU = frame->getBufU();
	uint8_t* tempBufV = frame->getBufV();

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