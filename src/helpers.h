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

Matrix3f matrixHfilter(Matrix3f H) {

    Matrix2f A;
    Matrix2f Q;
    Matrix2f R;
    Matrix2f QR;

    //cout << H << endl << endl;

    A << H(0, 0), H(0, 1), H(1, 0), H(1, 1);

    cout << A << endl << endl;
    
    ColPivHouseholderQR<Matrix2f> qr(A);
    qr.compute(A);

    Q = qr.matrixQ();
    R = qr.matrixR();

    R(0, 0) = (R(0, 0) + R(1, 1)) / 2;
    R(1, 1) = R(0, 0);
    R(0, 1) = 0;
    R(1, 0) = 0;

    cout << R << endl << endl;

    QR = Q * R;

    Matrix3f result;

    // sklejenie nowej macierzy H
    result(0, 0) = QR(0, 0);
    result(0, 1) = QR(0, 1);
    result(1, 0) = QR(1, 0);
    result(1, 1) = QR(1, 1);
    
    result(0, 2) = H(0, 2);
    result(1, 2) = H(1, 2);
    result(2, 0) = 0.0;
    result(2, 1) = 0.0;
    result(2, 2) = 1.0;
    
    return result;
}

Matrix3f calcMeanH(vector<Matrix3f> vH, int iFrameNo, int iFilterWindow) {

    vector<Matrix3f> vTempH;
    int iHCounter = 0;

    if (iFrameNo == 0) {
        return vH[0];
    }

    if (iFrameNo == 1) {
        return vH[1];
    }

    vTempH.push_back(vH[iFrameNo]);
    iHCounter++;

    for (int i = iFrameNo, j=0 ; i > 1; i--, j++) {
        
        if (iHCounter == iFilterWindow)
            break;

        Matrix3f tmp = vH[i - 1] * vTempH[j];

        tmp = matrixHfilter(tmp);

        vTempH.push_back(tmp);
        iHCounter++;
        
    }

    Matrix3f result;
    result << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

    // calculate mean of matrixes in vector vTempH
 
    for (int i = 0; i < vTempH.size(); i++) {
        result += vTempH[i];
    }

    result /= iHCounter;

    return result;
}

void saveMatrixHtoFile(Matrix3f &vHmatrix) {

    ofstream myfile;
    myfile.open("../H.txt", std::ios::app);
    myfile << vHmatrix(0, 0) << "\n";
    myfile << vHmatrix(0, 1) << "\n";
    myfile << vHmatrix(0, 2) << "\n";
    myfile << vHmatrix(1, 0) << "\n";
    myfile << vHmatrix(1, 1) << "\n";
    myfile << vHmatrix(1, 2) << "\n";
    myfile << vHmatrix(2, 0) << "\n";
    myfile << vHmatrix(2, 1) << "\n";
    myfile << vHmatrix(2, 2) << "\n";
    myfile.close();

}

vector<Matrix3f> readHmatrixHfromFile() {
    vector<Matrix3f> vHmatrix;

    fstream myInfile;
    myInfile.open("../H.txt", ios_base::in);
    float a, b, c, d, e, f, g, h, i;
    while (myInfile >> a >> b >> c >> d >> e >> f >> g >> h >> i) {
        Matrix3f tmp;
        tmp << a, b, c, d, e, f, g, h, i;
        //cout << tmp;
        vHmatrix.push_back(tmp);
    }
    myInfile.close();
    return vHmatrix;
}





