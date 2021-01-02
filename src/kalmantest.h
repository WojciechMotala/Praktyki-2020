#pragma once

using namespace std;

struct Trajectory
{
	Trajectory() {}
	Trajectory(double _x, double _y, double _a, double _s) {
		x = _x;
		y = _y;
		a = _a;
		s = _s;
	}
	// "+"
	friend Trajectory operator+(const Trajectory& c1, const Trajectory& c2) {
		return Trajectory(c1.x + c2.x, c1.y + c2.y, c1.a + c2.a, c1.s + c2.s);
	}
	//"-"
	friend Trajectory operator-(const Trajectory& c1, const Trajectory& c2) {
		return Trajectory(c1.x - c2.x, c1.y - c2.y, c1.a - c2.a, c1.s - c2.s);
	}
	//"*"
	friend Trajectory operator*(const Trajectory& c1, const Trajectory& c2) {
		return Trajectory(c1.x * c2.x, c1.y * c2.y, c1.a * c2.a, c1.s * c2.s);
	}
	//"/"
	friend Trajectory operator/(const Trajectory& c1, const Trajectory& c2) {
		return Trajectory(c1.x / c2.x, c1.y / c2.y, c1.a / c2.a, c1.s / c2.s);
	}
	//"="
	Trajectory operator =(const Trajectory& rx) {
		x = rx.x;
		y = rx.y;
		a = rx.a;
		s = rx.s;
		return Trajectory(x, y, a, s);
	}

	double x;
	double y;
	double a; // angle
	double s;
};

vector<Matrix3d> kTest(vector<Matrix3d> H) {

	// Accumulated frame to frame transform
	double a = 0;
	double x = 0;
	double y = 0;
	double s = 0;

	vector <Trajectory> smoothed_trajectory; // trajectory at all frames
	Trajectory X;//posteriori state estimate
	Trajectory	X_;//priori estimate
	Trajectory P;// posteriori estimate error covariance
	Trajectory P_;// priori estimate error covariance
	Trajectory K;//gain
	Trajectory	z;//actual measurement
	double pstd = 4e-3;//can be changed //4e-3 
	double cstd = 2.0;//can be changed //0.25
	Trajectory Q(2e-3, 2e-3, 16e-3, 2e-3);// process noise covariance
	Trajectory R(0.1, 0.1, 0.2, 0.1);// measurement noise covariance 

	int k = 1;

	vector<Matrix3d> result;

	FILE* fpX;
	fpX = fopen("X.txt", "w");
	FILE* fpY;
	fpY = fopen("Y.txt", "w");
	FILE* fpA;
	fpA = fopen("A.txt", "w");
	FILE* fpS;
	fpS = fopen("S.txt", "w");
	FILE* fpXe;
	fpXe = fopen("Xe.txt", "w");
	FILE* fpYe;
	fpYe = fopen("Ye.txt", "w");
	FILE* fpAe;
	fpAe = fopen("Ae.txt", "w");
	FILE* fpSe;
	fpSe = fopen("Se.txt", "w");

	for (int i = 0; i < H.size(); i++) {

		// decompose T
		double dx = H[i](0, 2);
		double dy = H[i](1, 2);
		double da = atan2(H[i](1, 0), H[i](0, 0));
		double ds = H[i](0, 0) / cos(da);

		

		x += dx;
		y += dy;
		a += da;
		s += ds;

		fprintf(fpX, "%f\n", x);
		fprintf(fpY, "%f\n", y);
		fprintf(fpA, "%f\n", a);
		fprintf(fpS, "%f\n", s);

		z = Trajectory(x, y, a, s);

		if (k == 1) {
			// intial guesses
			X = Trajectory(0, 0, 0, 0); //Initial estimate,  set 0
			P = Trajectory(1, 1, 1, 1); //set error variance,set 1
		}
		else
		{
			//time update（prediction）
			X_ = X; //X_(k) = X(k-1);
			P_ = P + Q; //P_(k) = P(k-1)+Q;
			// measurement update（correction）
			K = P_ / (P_ + R); //gain;K(k) = P_(k)/( P_(k)+R );
			X = X_ + K * (z - X_); //z-X_ is residual,X(k) = X_(k)+K(k)*(z(k)-X_(k)); 
			P = (Trajectory(1, 1, 1, 1) - K) * P_; //P(k) = (1-K(k))*P_(k);
		}

		// target - current
		double diff_x = X.x - x;//
		double diff_y = X.y - y;
		double diff_a = X.a - a;
		double diff_s = X.s - s;

		dx = dx + diff_x;
		dy = dy + diff_y;
		da = da + diff_a;
		ds = ds + diff_s;

		fprintf(fpXe, "%f\n", X.x);
		fprintf(fpYe, "%f\n", X.y);
		fprintf(fpAe, "%f\n", X.a);
		fprintf(fpSe, "%f\n", X.s);

		Matrix3d outH;
		outH(0, 0) = cos(da);
		outH(0, 1) = -sin(da);
		outH(0, 2) = dx;
		outH(1, 0) = sin(da);
		outH(1, 1) = cos(da);
		outH(1, 2) = dy;
		outH(2, 0) = 0.0;
		outH(2, 1) = 0.0;
		outH(2, 2) = 1.0;

		result.push_back(outH);

		k++;
	}

	fclose(fpX);
	fclose(fpY);
	fclose(fpA);
	fclose(fpS);
	fclose(fpXe);
	fclose(fpYe);
	fclose(fpAe);
	fclose(fpSe);

	return result;
}

vector<Matrix3d> kTestTQR(vector<Matrix3d> vT, vector<Matrix3d> vQ, vector<Matrix3d> vR) {

	// Accumulated frame to frame transform
	double a = 0;
	double x = 0;
	double y = 0;
	double s = 0;

	vector <Trajectory> smoothed_trajectory; // trajectory at all frames
	Trajectory X;//posteriori state estimate
	Trajectory	X_;//priori estimate
	Trajectory P;// posteriori estimate error covariance
	Trajectory P_;// priori estimate error covariance
	Trajectory K;//gain
	Trajectory	z;//actual measurement
	double pstd = 4e-3;//can be changed //4e-3 
	double cstd = 2.0;//can be changed //0.25
	Trajectory Q(1.5e-3, 1.5e-3, 1.5e-3, 2e-3);// process noise covariance
	Trajectory R(0.1, 0.3, 0.05, 0.3);// measurement noise covariance 

	int k = 1;

	vector<Matrix3d> result;

	/*
	FILE* fpX;
	fpX = fopen("X.txt", "w");
	FILE* fpY;
	fpY = fopen("Y.txt", "w");
	FILE* fpA;
	fpA = fopen("A.txt", "w");
	FILE* fpS;
	fpS = fopen("S.txt", "w");
	FILE* fpXe;
	fpXe = fopen("Xe.txt", "w");
	FILE* fpYe;
	fpYe = fopen("Ye.txt", "w");
	FILE* fpAe;
	fpAe = fopen("Ae.txt", "w");
	FILE* fpSe;
	fpSe = fopen("Se.txt", "w");
	*/

	for (int i = 0; i < vT.size(); i++) {

		// decompose T
		double dx = vT[i](0, 2);
		double dy = vT[i](1, 2);
		double da = atan2(vQ[i](1, 0), vQ[i](0, 0));
		double ds = vR[i](0, 0);
		double dds = ds;

		//cout << ds << endl;

		x += dx;
		y += dy;
		a += da;
		s += ds;

		/*
		fprintf(fpX, "%f\n", x);
		fprintf(fpY, "%f\n", y);
		fprintf(fpA, "%f\n", a);
		fprintf(fpS, "%f\n", s);
		*/

		z = Trajectory(x, y, a, s);

		if (k == 1) {
			// intial guesses
			X = Trajectory(0, 0, 0, 0); //Initial estimate,  set 0
			P = Trajectory(1, 1, 1, 1); //set error variance,set 1
		}
		else
		{
			//time update（prediction）
			X_ = X; //X_(k) = X(k-1);
			P_ = P + Q; //P_(k) = P(k-1)+Q;
			// measurement update（correction）
			K = P_ / (P_ + R); //gain;K(k) = P_(k)/( P_(k)+R );
			X = X_ + K * (z - X_); //z-X_ is residual,X(k) = X_(k)+K(k)*(z(k)-X_(k)); 
			P = (Trajectory(1, 1, 1, 1) - K) * P_; //P(k) = (1-K(k))*P_(k);
		}

		// target - current
		double diff_x = X.x - x;//
		double diff_y = X.y - y;
		double diff_a = X.a - a;
		double diff_s = X.s - s;

		dx = dx + diff_x;
		dy = dy + diff_y;
		da = da + diff_a;
		ds = ds + diff_s;

		/*
		fprintf(fpXe, "%f\n", X.x);
		fprintf(fpYe, "%f\n", X.y);
		fprintf(fpAe, "%f\n", X.a);
		fprintf(fpSe, "%f\n", X.s);
		*/

		Matrix3d outH;
		outH(0, 0) = cos(da);
		outH(0, 1) = -sin(da);
		outH(0, 2) = dx;
		outH(1, 0) = sin(da);
		outH(1, 1) = cos(da);
		outH(1, 2) = dy;
		outH(2, 0) = 0.0;
		outH(2, 1) = 0.0;
		outH(2, 2) = 1.0;

		result.push_back(outH);

		k++;
	}

	/*
	fclose(fpX);
	fclose(fpY);
	fclose(fpA);
	fclose(fpS);
	fclose(fpXe);
	fclose(fpYe);
	fclose(fpAe);
	fclose(fpSe);
	*/

	return result;
}