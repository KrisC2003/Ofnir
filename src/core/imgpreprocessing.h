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
	void medConfPreprocessing(cv::Mat& img);
	void lowConfPreprocessing(cv::Mat& img);
	void denoise(cv::Mat& img);
	void sharpen(cv::Mat& img);
	void adaptiveThresholding(cv::Mat& img);
	void morphologyClean(cv::Mat& img);
private:
	
};
