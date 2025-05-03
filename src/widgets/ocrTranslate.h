#pragma once

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>
#include <sstream>
#include <map>
#include "opencv2/opencv.hpp"
#include "src/core/imgpreprocessing.h" 

using json = nlohmann::json;

struct BlockData {
	std::string text;
	float confidence;
	std::vector<cv::Point> boundingBox;
};

class OCRManager {

public:

protected:
	std::wstring convertMultilangUTF8ToWstring(const std::string& str);
	std::string htmlEntityDecode(const std::string& input);
	std::string translateText(const std::string& text, const std::string& targetLang);
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

	std::string encodeImageToBase64(const std::string& imagePath);
	std::string fetchOCRResponse(const std::string& imagePath);
	std::string processOCRWithConfidence(const std::string& imagePath, float  confidenceLevel);
private:
	ImgPreprocessing imgProcessor;
	std::string apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  // API Key

};





