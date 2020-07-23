#pragma once
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;

class KalmanFilter {
private:
	MatrixXd m_X;
	MatrixXd m_X_min;
	MatrixXd m_Y;
	MatrixXd m_transition;
	MatrixXd m_measurement;
	MatrixXd m_covariance_P;
	MatrixXd m_covariance_Q;
	MatrixXd m_covariance_R;

public:

	KalmanFilter(MatrixXd X, MatrixXd F, MatrixXd H, MatrixXd P, MatrixXd Q, MatrixXd R);
	void predict();
	void update(MatrixXd z);
	MatrixXd predictAndUpdate(MatrixXd z);

};