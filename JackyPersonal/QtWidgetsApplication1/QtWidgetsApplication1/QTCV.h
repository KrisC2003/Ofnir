#pragma once
#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>

cv::Mat QPixmapToMat(const QPixmap& pixmap);
QImage MatToQImage(const cv::Mat& mat);
