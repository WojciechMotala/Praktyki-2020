#include "KalmanFilter.h"

KalmanFilter::KalmanFilter(MatrixXd X, MatrixXd F, MatrixXd H, MatrixXd P, MatrixXd Q, MatrixXd R) {
	m_X = X;
	m_X_min = X.setZero();
	m_Y = X.setZero();
	m_transition = F;
	m_measurement = H;
	m_covariance_P = P;
	m_covariance_Q = Q;
	m_covariance_R = R;
}

void KalmanFilter::predict() {
	m_X = m_transition * m_X;
	m_covariance_P = (m_transition * m_covariance_P) * (m_transition.transpose()) + m_covariance_Q;
}

void KalmanFilter::update(MatrixXd z) {
	MatrixXd S = (m_measurement * m_covariance_P) * m_measurement.transpose() + m_covariance_R;
	MatrixXd K = (m_covariance_P * m_measurement.transpose()) * S.inverse();

	MatrixXd y = z - (m_measurement * m_X);

	m_X += K * y;

	m_covariance_P -= (K * m_measurement) * m_covariance_P;
}

MatrixXd KalmanFilter::predictAndUpdate(MatrixXd z) {
	predict();
	update(z);
	return m_X;
}

