#pragma once
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

class imgpreprocessing {
public:
	QImage MatToQImg(const cv::Mat& mat);
	void preprocessImg(cv::Mat& img);
protected:
	void highConfPreprocessing();
	void medConfPreprocessing();
	void lowConfPreprocessing();
private:
	cv::Mat m_targetImg;
	
};
