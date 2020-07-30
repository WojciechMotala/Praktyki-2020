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

Matrix3d calcHmat(list<ezsift::MatchPair> match_list) {

    list<ezsift::MatchPair>::iterator pair;

    int iM = 3;
    int iN = match_list.size();

    MatrixXd M1(iM, iN);
    MatrixXd M2(iM, iN);

    int iCol;

    // populate matrixes
    for (iCol = 0, pair = match_list.begin(); pair != match_list.end(); pair++, iCol++) {

        M1(0, iCol) = pair->c1;
        M1(1, iCol) = pair->r1;
        M1(2, iCol) = 1;

        M2(0, iCol) = pair->c2;
        M2(1, iCol) = pair->r2;
        M2(2, iCol) = 1;
    }

    Matrix3d H = (M2 * M1.transpose()) * (M1 * M1.transpose()).inverse();

    //cout << H;
    return H;
}

Matrix3f matrixHfilter(Matrix3f H) {

    Matrix2f A;
    Matrix2f Q;
    Matrix2f R;
    Matrix2f QR;
    float a, b, c, d, 
          q1, q2, q3, q4, 
          r1, r2, r3, r4;

    /**
    *
    * A = | a, b |
    *     | c, d |
    *
    * Q = | q1, q2 | 
    *     | q3, q4 |
    *
    * R = | r1, r2 |
    *     | r3, r4 |
    *
    **/


    //cout << H << endl << endl;

    //A << H(0, 0), H(0, 1), H(1, 0), H(1, 1);
    a = H(0, 0);
    b = H(0, 1);
    c = H(1, 0);
    d = H(1, 1);
    /*
    a = 3;
    b = 3;
    c = 4;
    d = 5;
    */
    float tmpSqrt = sqrtf((b * c * b * c - a * d * b * c - b * c * a * d + a * d * a * d) / (a * a + c * c));

    q1 = a / sqrtf((a * a) + (c * c));
    q2 = c / sqrtf((a * a) + (c * c));
    q3 = (c * b * c - c * a * d) / ((a * a + c * c) * tmpSqrt);
    q4 = (a * a * d - a * b * c) / ((a * a + c * c) * tmpSqrt);

    r1 = (sqrtf(a * a + c * c) + tmpSqrt) / 2;
    //r2 = (b * a + d * c) / sqrtf(a * a + c * c);
    r2 = 0.0;
    r3 = 0.0;
    r4 = r1;
    
    Q << q1, q2, q3, q4;
    R << r1, r2, r3, r4;
    
    //cout << Q << endl << endl << R << endl << endl;

    QR = Q * R;
    //cout << QR << endl << endl;
    Matrix3f result;

    // sklejenie nowej macierzy H
    result(0, 0) = QR(0, 0);
    result(0, 1) = QR(0, 1);
    result(1, 0) = QR(1, 0);
    result(1, 1) = QR(1, 1);
    
    result(0, 2) = H(0, 2);
    result(1, 2) = H(1, 2);
    result(2, 0) = H(2, 0);//0.0;
    result(2, 1) = H(2, 1);//0.0;
    result(2, 2) = H(2, 2);//1.0;
    //cout << result << endl << endl;
    return result;

}

Matrix3d calcMeanH(vector<Matrix3d> vH, int iFrameNo, int iFilterWindow) {

    vector<Matrix3d> vTempH;
    int iHCounter = 0;

    if (iFrameNo == 0) {
        return vH[0];
    }
    /*
    if (iFrameNo == 1) {
        return vH[1];
    }
    */
    vTempH.push_back(vH[iFrameNo]);
    iHCounter++;

    for (int i = iFrameNo, j=0 ; i > 0; i--, j++) {
        
        if (iHCounter == iFilterWindow)
            break;

        Matrix3d tmp = vH[i - 1] * vTempH[j];

        //tmp = matrixHfilter(tmp);

        vTempH.push_back(tmp);
        iHCounter++;
        
    }

    Matrix3d result;
    //result << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0;

    // calculate mean of matrixes in vector vTempH
 
    for (int i = 0; i < vTempH.size(); i++) {
        result += vTempH[i];
    }

    result /= iHCounter;

    return result;
}

void calcMeanTQR(vector<Matrix3d> &vT, vector<Matrix3d> &vQ, vector<Matrix3d> &vR, vector<Matrix3d> &vMeanT, vector<Matrix3d> &vMeanQ, vector<Matrix3d> &vMeanR, int iFrameNo, int iFilterWindowT, int iFilterWindowQ, int iFilterWindowR) {

    vector<Matrix3d> vTempT;
    vector<Matrix3d> vTempQ;
    vector<Matrix3d> vTempR;
    int iTCounter = 0;
    int iQCounter = 0;
    int iRCounter = 0;

    if (iFrameNo == 0) {
        //return vH[0];
        vMeanT.push_back(vT[0]);
        vMeanQ.push_back(vQ[0]);
        vMeanR.push_back(vR[0]);
        return;
    }

    /*
    if (iFrameNo == 1) {
        vMeanT.push_back(vT[1]);
        vMeanQ.push_back(vQ[1]);
        vMeanR.push_back(vR[1]);
        return;
    }
    */

    vTempT.push_back(vT[iFrameNo]);
    vTempQ.push_back(vQ[iFrameNo]);
    vTempR.push_back(vR[iFrameNo]);
    
    iTCounter++;
    iQCounter++;
    iRCounter++;

    for (int i = iFrameNo, j = 0; i > 0; i--, j++) {

        if (iTCounter == iFilterWindowT)
            break;

        Matrix3d tmp = vT[i - 1] * vTempT[j];

        vTempT.push_back(tmp);
        iTCounter++;

    }

    for (int i = iFrameNo, j = 0; i > 0; i--, j++) {

        if (iQCounter == iFilterWindowQ)
            break;

        Matrix3d tmp = vQ[i - 1] * vTempQ[j];

        vTempQ.push_back(tmp);
        iQCounter++;

    }

    for (int i = iFrameNo, j = 0; i > 0; i--, j++) {

        if (iRCounter == iFilterWindowR)
            break;

        Matrix3d tmp = vR[i - 1] * vTempR[j];

        vTempR.push_back(tmp);
        iRCounter++;

    }

    Matrix3d resultT;
    Matrix3d resultQ;
    Matrix3d resultR;
    
    // calculate mean of matrixes in vector vTempH

    for (int i = 0; i < vTempT.size(); i++) {
        resultT += vTempT[i];
    }
    for (int i = 0; i < vTempQ.size(); i++) {
        resultQ += vTempQ[i];
    }
    for (int i = 0; i < vTempR.size(); i++) {
        resultR += vTempR[i];
    }

    resultT /= iTCounter;
    resultQ /= iQCounter;
    resultR /= iRCounter;

    vMeanT.push_back(resultT);
    vMeanQ.push_back(resultQ);
    vMeanR.push_back(resultR);
}

void saveMatrixHtoFile(Matrix3d &vHmatrix) {

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

vector<Matrix3d> readHmatrixfromFile() {
    vector<Matrix3d> vHmatrix;

    fstream myInfile;
    myInfile.open("../H_srednie.txt", ios_base::in);
    double a, b, c, d, e, f, g, h, i;
    while (myInfile >> a >> b >> c >> d >> e >> f >> g >> h >> i) {
        Matrix3d tmp;
        tmp << a, b, c, d, e, f, g, h, i;
        vHmatrix.push_back(tmp);
    }
    myInfile.close();
    return vHmatrix;
}

void matrixFactorisationH(vector<Matrix3d> &vT, vector<Matrix3d> &vQ, vector<Matrix3d> &vR, Matrix3d H) {

    // Translation Matrix
    Matrix3d T;
    T(0, 0) = 1.0;
    T(0, 1) = 0.0;
    T(0, 2) = H(0, 2);
    
    T(1, 0) = 0.0;
    T(1, 1) = 1.0;
    T(1, 2) = H(1, 2);

    T(2, 0) = 0.0;
    T(2, 1) = 0.0;
    T(2, 2) = 1.0;

    // Rotation and Scale Matrix
 
    Matrix3d Q;
    Matrix3d R;
    Matrix2d tmpQ;
    Matrix2d tmpR;

    double a, b, c, d,
           q1, q2, q3, q4,
           r1, r2, r3, r4;

    /**
    *
    * A = | a, b |
    *     | c, d |
    *
    * Q = | q1, q2, 0 |
    *     | q3, q4, 0 |
    *     |  0,  0, 1 |
    *
    * R = | r1, r2, 0 |
    *     | r3, r4, 0 |
    *     |  0,  0, 1 |
    *
    **/

    a = H(0, 0);
    b = H(0, 1);
    c = H(1, 0);
    d = H(1, 1);
    /*
    float tmpSqrt = sqrtf((b * c * b * c - a * d * b * c - b * c * a * d + a * d * a * d) / (a * a + c * c));

    q1 = a / sqrtf((a * a) + (c * c));
    q2 = c / sqrtf((a * a) + (c * c));
    q3 = (c * b * c - c * a * d) / ((a * a + c * c) * tmpSqrt);
    q4 = (a * a * d - a * b * c) / ((a * a + c * c) * tmpSqrt);
    */
    q1 = a / (sqrt((a * a) + (c * c)));
    q2 = -c / (sqrt((a * a) + (c * c)));
    q3 = c / (sqrt((a * a) + (c * c)));
    q4 = a / (sqrt((a * a) + (c * c)));

    /*
    r1 = (sqrtf(a * a + c * c) + tmpSqrt) / 2.0;
    r2 = 0.0;
    r3 = 0.0;
    r4 = r1;
    */
    r1 = (sqrt((a * a) + (c * c)) + (a * d - b * c) / (sqrt((a * a) + (c * c)))) / 2;
    r2 = 0.0;
    r3 = 0.0;
    r4 = r1;

    //tmpQ << q1, q2, q3, q4;
    //tmpR << r1, r2, r3, r4;

    Q(0, 0) = q1;//tmpQ(0, 0);
    Q(0, 1) = q2;//tmpQ(0, 1);
    Q(0, 2) = 0.0;

    Q(1, 0) = q3;//tmpQ(1, 0);
    Q(1, 1) = q4;//tmpQ(1, 1);
    Q(1, 2) = 0.0;

    Q(2, 0) = 0.0;
    Q(2, 1) = 0.0;
    Q(2, 2) = 1.0;
    
    R(0, 0) = r1;//tmpR(0, 0);
    R(0, 1) = 0.0;
    R(0, 2) = 0.0;

    R(1, 0) = 0.0;
    R(1, 1) = r4;//tmpR(1, 1);
    R(1, 2) = 0.0;

    R(2, 0) = 0.0;
    R(2, 1) = 0.0;
    R(2, 2) = 1.0;
    
    vT.push_back(T);
    vQ.push_back(Q);
    vR.push_back(R);

    //cout << T << endl << Q << endl << R << endl << endl;


    //=============================================================================
    // H compare
    //=============================================================================
    /*
    FILE* fp;
    fp = fopen("H_th_05.txt", "a");

    double theta = atan2(q3, q4) * 180 / 3.14159265358979323846;
    double scale = r1;
    double dx = T(0, 2);
    double dy = T(1, 2);

    fprintf(fp, "Theta: %10f Scale: %10f dx: %10f dy: %10f\n", theta, scale, dx, dy);

    fclose(fp);
    */
    //=============================================================================

}

MatrixXd getCovFromH(vector<MatrixXd> vH) {

    int iN = vH.size();

    // rows -> observation, cols -> feature 
    MatrixXd params(iN, 6); // N -> number of frames 

    for (int i = 0; i < vH.size(); i++) {
        params(i, 0) = vH[i](0, 0); //Q1
        params(i, 1) = vH[i](0, 1); //Q2
        params(i, 2) = vH[i](0, 2); //T1
        params(i, 3) = vH[i](1, 0); //Q3
        params(i, 4) = vH[i](1, 1); //Q4
        params(i, 5) = vH[i](1, 2); //T2
    }

    MatrixXd centered = params.rowwise() - params.colwise().mean();
    MatrixXd cov = (centered.adjoint() * centered) / float(params.rows() - 1);
    
    return cov;
}

MatrixXd sum2affine(MatrixXd a1, MatrixXd a2) {
    
    Matrix2d A1;
    A1(0, 0) = a1(0, 0);
    A1(0, 1) = a1(0, 1);
    A1(1, 0) = a1(1, 0);
    A1(1, 1) = a1(1, 1);

    MatrixXd b1(2, 1);
    b1(0, 0) = a1(0, 2);
    b1(1, 0) = a1(1, 2);

    Matrix2d A2;
    A2(0, 0) = a2(0, 0);
    A2(0, 1) = a2(0, 1);
    A2(1, 0) = a2(1, 0);
    A2(1, 1) = a2(1, 1);

    MatrixXd b2(2, 1);
    b2(0, 0) = a2(0, 2);
    b2(1, 0) = a2(1, 2);

    Matrix2d A = A2 * A1;
    MatrixXd b(2, 1);
    b = (A2 * b1) + b2;

    MatrixXd result(2, 3);

    result(0, 0) = A(0, 0);
    result(0, 1) = A(0, 1);
    result(0, 2) = b(0, 0);
    result(1, 0) = A(1, 0);
    result(1, 1) = A(1, 1);
    result(1, 2) = b(1, 0);

    return result;
}

MatrixXd matrixHnorm(Matrix3d H) {

    MatrixXd temp(2, 3);

    temp(0, 0) = H(0, 0);
    temp(0, 1) = H(0, 1);
    temp(0, 2) = H(0, 2);
    temp(1, 0) = H(1, 0);
    temp(1, 1) = H(1, 1);
    temp(1, 2) = H(1, 2);

    return temp;
}

MatrixXd reshape(MatrixXd source) {

    MatrixXd temp(2, 3);

    temp(0, 0) = source(0, 0);
    temp(0, 1) = source(1, 0);
    temp(0, 2) = source(2, 0);
    temp(1, 0) = source(3, 0);
    temp(1, 1) = source(4, 0);
    temp(1, 2) = source(5, 0);

    return temp;

}

MatrixXd compensatingTransform(MatrixXd original, MatrixXd next) {

    MatrixXd nextBlock = next.block<2, 2>(0, 0);
    MatrixXd originalBlock = original.block<2, 2>(0, 0).inverse();
    MatrixXd A = nextBlock * originalBlock;

    MatrixXd nextBlockV = next.block<2, 1>(0, 2);
    MatrixXd originalBlockV = original.block<2, 1>(0, 2);

    MatrixXd b = ( -A * originalBlockV) + nextBlockV;

    MatrixXd tmp;
    tmp = A.inverse();
    A = tmp;

    b = -A * b;

    MatrixXd temp(2, 3);

    temp(0, 0) = A(0, 0);
    temp(0, 1) = A(0, 1);
    temp(0, 2) = b(0, 0);
    temp(1, 0) = A(1, 0);
    temp(1, 1) = A(1, 1);
    temp(1, 2) = b(1, 0);

    return temp;
}