#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <windows.h>
#include <locale>
#include <codecvt>
#include "opencv2/opencv.hpp"

using json = nlohmann::json;

// curl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// utf8 to wide string
std::wstring utf8ToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

// calling Google Translate
std::string translateText(const std::string& text, const std::string& targetLang) {
    std::string apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  // feel free to steal my key :P
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();
    if (curl) {
        char* encodedText = curl_easy_escape(curl, text.c_str(), 0);
        if (!encodedText) return "";

        std::string url = "https://translation.googleapis.com/language/translate/v2?key=" + apiKey +
            "&q=" + encodedText + "&target=" + targetLang;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        curl_free(encodedText);
        curl_easy_cleanup(curl);
    }

    try {
        auto jsonResponse = json::parse(response);
        return jsonResponse["data"]["translations"][0]["translatedText"];
    }
    catch (...) {
        return "";
    }
}




int main() {
    SetConsoleOutputCP(CP_UTF8);  //show chinese and japanese, not working tho lamo
    SetConsoleCP(CP_UTF8);


    std::string imagePath;
    std::cout << " Enter the image path: "; //you can type test.png to try out first, its in the folder
    std::getline(std::cin, imagePath);

    cv::Mat img = cv::imread(imagePath);
    if (img.empty()) {
        std::cerr << "can't read the photo, Please check Path¡£\n";
        return 1;
    }

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);           // Grayscale
    cv::GaussianBlur(img, img, cv::Size(5, 5), 0);        // Denoising
    cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);  // Binarization


    Pix* pixImage = pixCreate(img.cols, img.rows, 8);
    for (int i = 0; i < img.rows; i++) {
        for (int j = 0; j < img.cols; j++) {
            pixSetPixel(pixImage, j, i, img.at<uchar>(i, j));
        }
    }


    /* test with photos in projrct file
   Pix* image = pixRead(R"(test.png)");
   if (!image) {
       std::cerr << "can't read the photo, Please check Path¡£\n";
       return 1;
   }
   */

   //OCR
    tesseract::TessBaseAPI api;
    if (api.Init(NULL, "jpn")) {
        std::cerr << "Tesseract Startup fail\n";
        return 1;
    }

    api.SetImage(pixImage);
    api.Recognize(0);

    tesseract::ResultIterator* ri = api.GetIterator();
    tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;

    std::string ocrResult;

    if (ri != nullptr) {
        do {
            const char* line = ri->GetUTF8Text(level);
            if (line) {
                ocrResult += line;
                ocrResult += "\n";
                delete[] line;
            }
        } while (ri->Next(level));
    }

    std::wcout.imbue(std::locale("chs"));
    std::wcout << L"\n OCR result£º\n" << utf8ToWstring(ocrResult) << std::endl;

    // translation
    std::string translated = translateText(ocrResult, "en");
    std::wcout << L"\n translation result£º\n" << utf8ToWstring(translated) << std::endl;

    api.End();
    pixDestroy(&pixImage);

    return 0;
}
