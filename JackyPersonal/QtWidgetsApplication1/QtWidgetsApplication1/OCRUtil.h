#pragma once

#include <QPixmap>
#include <QImage>
#include <opencv2/opencv.hpp>
#include <string>
#include <windows.h>
#include <nlohmann/json.hpp>
#include <QColor>

using json = nlohmann::json;

// Image conversion
cv::Mat QPixmapToMat(const QPixmap& pixmap);
QImage MatToQImage(const cv::Mat& mat);

// Encoding utilities
std::string encodeImageToBase64(const std::string& imagePath);

// String utilities
std::wstring utf8ToWstring(const std::string& str);
std::string htmlEntityDecode(const std::string& input);
std::string insertLineBreaks(const std::string& text, size_t maxLineLength = 40);

// CURL utils
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

// Google API
std::string performOCRWithGoogleVision(const std::string& imagePath, float confidenceLevel);
std::string translateText(const std::string& text, const std::string& targetLang);
