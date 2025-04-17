#include "imgpreprocessing.h"

cv::Mat imgpreprocessing::QPixmapToMat(const QPixmap& pixmap)
{
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(image.height(), image.width(), CV_8UC3,
        const_cast<uchar*>(image.bits()), image.bytesPerLine());
    return mat.clone();
}

QImage imgpreprocessing::MatToQImage(const cv::Mat& mat)
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
// TODO: create a check for settings to apply different filters based on language
void imgpreprocessing::preprocessImg(cv::Mat& img, float confidence) {
    m_targetImg = img;
    if (img.empty()) {
        std::cerr << "failed to load img" << std::endl;
        return;
    }

    if (confidence > 90) {
        highConfPreprocessing();
    }
    return;
}

//basic img preprocessing, applies greyscale then binary threshold
void imgpreprocessing::highConfPreprocessing() {
    cv::cvtColor(m_targetImg, m_targetImg, cv::COLOR_RGB2GRAY);
    cv::threshold(m_targetImg, m_targetImg, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    return;
}

void imgpreprocessing::medConfPreprocessing() {
    return;
}

void imgpreprocessing::lowConfPreprocessing() {
    return;
}