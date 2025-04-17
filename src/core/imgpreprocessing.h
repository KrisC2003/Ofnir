#pragma once
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

class imgpreprocessing {
public:
	cv::Mat QPixmapToMat(const QPixmap& pixmap);
	QImage MatToQImage(const cv::Mat& mat);
	void preprocessImg(cv::Mat& img, float confidence);
protected:
	void highConfPreprocessing();
	void medConfPreprocessing();
	void lowConfPreprocessing();
private:
	cv::Mat m_targetImg;
	
};
