#include "imgpreprocessing.h"

enum class ConfidenceTier {
    High,
    Medium,
    Low,
    VeryLow
};

ConfidenceTier categorizeConfidence(float confidence) {
    if (confidence >= 0.85f) return ConfidenceTier::High;
    if (confidence >= 0.8f) return ConfidenceTier::Medium;
    if (confidence >= 0.7f) return ConfidenceTier::Low;
    return ConfidenceTier::VeryLow;
}

cv::Mat ImgPreprocessing::QPixmapToMat(const QPixmap& pixmap)
{
    QImage image = pixmap.toImage().convertToFormat(QImage::Format_RGB888);
    cv::Mat mat(image.height(), image.width(), CV_8UC3,
        const_cast<uchar*>(image.bits()), image.bytesPerLine());
    return mat.clone();
}

QImage ImgPreprocessing::MatToQImage(const cv::Mat& mat)
{
    if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    else if (mat.type() == CV_8UC3) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped().copy();
    }
    return QImage();
}

// initial preprocessing
// TODO: inital preprocess is too harsh on slightly blurry images etc.
void ImgPreprocessing::initPreprocessImg(cv::Mat& img) {
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::threshold(img, img, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
}

// TODO: make it a regular preprocessing tree based on confidence results, ideally modular REFERENCE features/preprocessing
// TODO: create a check for settings to apply different filters based on language
// returns true if retry is necessary, false otherwise
bool ImgPreprocessing::preprocessImg(cv::Mat& img, float confidence) {
    if (img.empty()) {
        std::cerr << "failed to load img" << std::endl;
        return true;
    }
    switch (categorizeConfidence(confidence)) {
        case ConfidenceTier::High:
            return false;
        case ConfidenceTier::Medium:
            // partial pipeline (add Sharpening, Bilateral if needed)
            medConfPreprocessing();
            return true;
        case ConfidenceTier::Low:
            // Apply full pipeline
            lowConfPreprocessing();
            return true;
        case ConfidenceTier::VeryLow:
            // Trigger manual intervention or fallback
            break;
    }

    return false;
}
void ImgPreprocessing::medConfPreprocessing() {

    return;
}

void ImgPreprocessing::lowConfPreprocessing() {
    return;
}