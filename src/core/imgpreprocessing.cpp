#include "imgpreprocessing.h"

cv::Mat QPixmapToMat(const QPixmap& pixmap)
{
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(image.height(), image.width(), CV_8UC3,
        const_cast<uchar*>(image.bits()), image.bytesPerLine());
    return mat.clone();
}

QImage MatToQImage(const cv::Mat& mat)
{
    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    else if (mat.type() == CV_8UC3) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped().copy();
    }
    return QImage();
}

// TODO: make it a regular preprocessing tree based on confidence results, ideally modular REFERENCE features/preprocessing
void preprocessImg(cv::Mat& img, float confidence) {
    m_targetImg = img;
    if (img.empty()) {
        std::cerr << "failed to load img" << std::endl;
        return;
    }

    if (confidence > 90) {
        highConfPreprocessing(img);
    }

}

//basic img preprocessing, applies greyscale then binary threshold
void highConfPreprocessing(cv::Mat img) {
    cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
    cv::threshold(img, img, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return;
}

void medConfPreprocessing(cv::Mat img) {

}

void lowConfPreprocessing(cv::Mat img) {

}