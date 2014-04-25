#include "ExtendedKalmanFilter.h"

ExtendedKalmanFilter::ExtendedKalmanFilter(float _Q, float _Rgps, float _Rimu, float _Renc, float _dt) {
	this->Q = cv::Mat::eye(7, 7, CV_32F) * _Q;

	this->Rgps = cv::Mat::eye(2, 2, CV_32F) * _Rgps;
	this->Rimu = cv::Mat::eye(1, 1, CV_32F) * _Rimu;
	this->Renc = cv::Mat::eye(1, 1, CV_32F) * _Renc;

	this->I = cv::Mat::eye(4, 4, CV_32F);
	this->K = cv::Mat::zeros(7, 4, CV_32F);
	this->dt = _dt;
	this->x_apriori = cv::Mat::zeros(7, 1, CV_32F);
	this->x_aposteriori = cv::Mat::zeros(7, 1, CV_32F);
	this->P_apriori = cv::Mat::zeros(7, 7, CV_32F);
	this->P_aposteriori = cv::Mat::zeros(7, 7, CV_32F);

	this->Hgps = cv::Mat::zeros(2, 4, CV_32F);
	this->Hgps.at<float>(0, 0) = 1.0;
	this->Hgps.at<float>(1, 1) = 1.0;

	this->Himu = cv::Mat::zeros(1, 4, CV_32F);
	this->Himu.at<float>(0, 3) = 1.0;

	this->Henc = cv::Mat::zeros(1, 4, CV_32F);
	this->Henc.at<float>(0, 2) = 1.0;
}

cv::Mat ExtendedKalmanFilter::jacobian() {

	cv::Mat F = cv::Mat::zeros(4, 4, CV_32F);

	// 1st row
//	F.at<float>(0, 0) = 1.0;
//	F.at<float>(0, 1) = -0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6))
//			* this->dt;
//	F.at<float>(0, 2) = 0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
//	F.at<float>(0, 3) = 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
//
//	// 2nd row
//	F.at<float>(1, 0) = 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
//	F.at<float>(1, 1) = 1;
//	F.at<float>(1, 2) = - 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
//	F.at<float>(1, 3) = 0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
//
//	// 3rd row
//	F.at<float>(2, 0) = -0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
//	F.at<float>(2, 1) = 0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
//	F.at<float>(2, 2) = 1;
//	F.at<float>(2, 3) = 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
//
//	// 4th row
//	F.at<float>(3, 0) = -0.5 * (w.at<float>(0) - this->x_apriori.at<float>(4)) * this->dt;
//	F.at<float>(3, 1) =	- 0.5 * (w.at<float>(1) - this->x_apriori.at<float>(5)) * this->dt;
//	F.at<float>(3, 2) =	- 0.5 * (w.at<float>(2) - this->x_apriori.at<float>(6)) * this->dt;
//	F.at<float>(3, 3) = 1;

	return F;
}

cv::Mat ExtendedKalmanFilter::state() {

	cv::Mat F = cv::Mat::zeros(4, 1, CV_32F);

	// Only (1:4)
	cv::Mat x(this->x_aposteriori, cv::Rect(0, 0, 1, 4));
//	x = A * x;
//
//	F.at<float>(0) = x.at<float>(0);
//	F.at<float>(1) = x.at<float>(1);
//	F.at<float>(2) = x.at<float>(2);
//	F.at<float>(3) = x.at<float>(3);

	return x;
}

void ExtendedKalmanFilter::predict(float _dt) {
	this->dt = _dt;
	this->x_apriori = this->state();

	cv::Mat F = this->jacobian();
	this->P_apriori = F * this->P_aposteriori * F.t() + this->Q;
}
;

void ExtendedKalmanFilter::correctGPS(cv::Mat gpsMeasurement) {
	this->K = (this->P_apriori * this->Hgps.t())
			* (this->Hgps * this->P_apriori * this->Hgps.t() + this->Rgps).inv();
	this->x_aposteriori = this->x_apriori
			+ this->K * (gpsMeasurement - this->Hgps * this->x_apriori);
	this->P_aposteriori = (this->I - this->K * this->Hgps) * this->P_apriori;
}
;

void ExtendedKalmanFilter::correctIMU(cv::Mat imuMeasurement) {
	this->K = (this->P_apriori * this->Himu.t())
			* (this->Himu * this->P_apriori * this->Himu.t() + this->Rimu).inv();
	this->x_aposteriori = this->x_apriori
			+ this->K * (imuMeasurement - this->Himu * this->x_apriori);
	this->P_aposteriori = (this->I - this->K * this->Himu) * this->P_apriori;
}
;

void ExtendedKalmanFilter::correctEncoder(cv::Mat encMeasurement) {
	this->K = (this->P_apriori * this->Henc.t())
			* (this->Henc * this->P_apriori * this->Henc.t() + this->Renc).inv();
	this->x_aposteriori = this->x_apriori
			+ this->K * (encMeasurement - this->Henc * this->x_apriori);
	this->P_aposteriori = (this->I - this->K * this->Henc) * this->P_apriori;
}
;