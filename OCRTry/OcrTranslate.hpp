
// ocr_translate.hpp
#pragma once

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <locale>
#include <codecvt>
#include "opencv2/opencv.hpp"
#include <fstream>
#include <sstream>
#include <map>

using json = nlohmann::json;

// curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

// utf8 to wide string
std::wstring utf8ToWstring(const std::string& str);

// HTML entity decode
std::string htmlEntityDecode(const std::string& input);

// Google Translate API
std::string translateText(const std::string& text, const std::string& targetLang);

// Read image file and encode to base64
std::string encodeImageToBase64(const std::string& imagePath);

// Google Cloud Vision API OCR
std::string performOCRWithGoogleVision(const std::string& imagePath);
