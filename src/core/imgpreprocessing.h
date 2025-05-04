#pragma once
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

class ImgPreprocessing {
public:
	cv::Mat QPixmapToMat(const QPixmap& pixmap);
	QImage MatToQImage(const cv::Mat& mat);

	void initPreprocessImg(cv::Mat& img);
	bool preprocessImg(cv::Mat& img, float confidence);
protected:
	void medConfPreprocessing();
	void lowConfPreprocessing();
private:
	
};
