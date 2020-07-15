#include <Eigen/Dense>
using namespace std;
using namespace Eigen;

int* moveCompensation(list<ezsift::MatchPair> match_list) {

    double meanX = 0;
    double meanY = 0;

    list<ezsift::MatchPair>::iterator pair;
    int counterX = 0;
    int counterY = 0;

    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;

        if ((pair->c1 - pair->c2) != 0) {
            meanX += pair->c1 - pair->c2;
            counterX++;
        }

        if ((pair->r1 - pair->r2) != 0) {
            meanY += pair->r1 - pair->r2;
            counterY++;
        }

    }

    meanX /= counterX;
    meanY /= counterY;

    int resX = round(meanX);
    int resY = round(meanY);

    int iCompensationXY[2];
    iCompensationXY[0] = resX;
    iCompensationXY[1] = resY;

    return iCompensationXY;

}

float calculateRotationAngle(list<ezsift::MatchPair> match_list) {

    list<ezsift::MatchPair>::iterator pair;

    float fAngle = 0.0;
    int iCounter = 0;

    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;

        float Theta1 = atan2((float)pair->r1, (float)pair->c1);
        float Theta2 = atan2((float)pair->r2, (float)pair->c2);
        float fTheta = Theta1 - Theta2;

        fAngle += fTheta;
        iCounter++;
    }

    if (iCounter != 0)
        fAngle /= iCounter;

    return fAngle;
}

float calculateCenterRotationAngle(list<ezsift::MatchPair> match_list, int width, int height) {

    list<ezsift::MatchPair>::iterator pair;

    float fAngle = 0.0;
    int iCounter = 0;

    int iCenterX = width / 2;
    int iCenterY = height / 2;

    // loop through every matched pair
    for (pair = match_list.begin(); pair != match_list.end(); pair++) {

        if ((pair->c1 == pair->c2) && (pair->r1 == pair->r2))
            continue;

        // slope of a straight line
        float fA1 = ((float)iCenterY - (float)pair->r1) / ((float)iCenterX - (float)pair->c1);
        float fA2 = ((float)iCenterY - (float)pair->r2) / ((float)iCenterX - (float)pair->c2);

        float fThetaA1 = atan(fA1);
        float fThetaA2 = atan(fA2);

        float fTheta = fThetaA1 - fThetaA2;

        fAngle += fTheta;
        iCounter++;
    }

    fAngle /= iCounter;

    return fAngle;
}

void mulMat(float rotMat[], float theta) {
    float mat[4] = { cos(theta), -sin(theta),
                     sin(theta),  cos(theta) };

    float tempMat[4] = { rotMat[0], rotMat[1],
                         rotMat[2], rotMat[3] };


    rotMat[0] = tempMat[0] * mat[0] + tempMat[1] * mat[2];
    rotMat[1] = tempMat[0] * mat[1] + tempMat[1] * mat[3];
    rotMat[2] = tempMat[2] * mat[0] + tempMat[3] * mat[2];
    rotMat[3] = tempMat[2] * mat[1] + tempMat[3] * mat[3];

}

Matrix3f calcHmat(list<ezsift::MatchPair> match_list) {

    list<ezsift::MatchPair>::iterator pair;

    int iM = 3;
    int iN = match_list.size();

    MatrixXf M1(iM, iN);
    MatrixXf M2(iM, iN);

    int iCol;

    // populate matrixes
    for (iCol = 0, pair = match_list.begin(); pair != match_list.end(); pair++, iCol++) {

        M1(0, iCol) = pair->c2;
        M1(1, iCol) = pair->r2;
        M1(2, iCol) = 1;

        M2(0, iCol) = pair->c1;
        M2(1, iCol) = pair->r1;
        M2(2, iCol) = 1;
    }

    Matrix3f H = (M2 * M1.transpose()) * (M1 * M1.transpose()).inverse();

    //cout << H;
    return H;
}

