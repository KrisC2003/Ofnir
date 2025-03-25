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
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// utf8 to wide string
std::wstring utf8ToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}
//decode result 
std::string htmlEntityDecode(const std::string& input) {
    std::map<std::string, char> html_entities = {
        {"&quot;", '\"'}, {"&apos;", '\''}, {"&amp;", '&'},
        {"&lt;", '<'}, {"&gt;", '>'}, {"&#39;", '\''}
    };

    std::string output = input;
    for (const auto& pair : html_entities) {
        size_t pos = 0;
        while ((pos = output.find(pair.first, pos)) != std::string::npos) {
            output.replace(pos, pair.first.length(), std::string(1, pair.second));
            pos += 1;
        }
    }
    return output;
}


// calling Google Translate
std::string translateText(const std::string& text, const std::string& targetLang) {
    std::string apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  // feel free to steal my key :P
    CURL* curl;
    //CURLcode res;
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
        return htmlEntityDecode(jsonResponse["data"]["translations"][0]["translatedText"]);
    }
    catch (...) {
        return "";
    }
}

// read image file and encode to base64
std::string encodeImageToBase64(const std::string& imagePath) {
    std::ifstream file(imagePath, std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string imageData = oss.str();

    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string encoded;
    int val = 0, valb = -6;
    for (uint8_t c : imageData) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(table[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(table[((val << 8) >> (valb + 8)) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');
    return encoded;
}
// calling Google Cloud Vision API
std::string performOCRWithGoogleVision(const std::string& imagePath) {
    std::string apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  //API Key
    std::string base64Image = encodeImageToBase64(imagePath);

    json requestBody = {
        {"requests", {{
            {"image", {{"content", base64Image}}},
            {"features", {{{"type", "TEXT_DETECTION"}}}}
        }}}
    };
    std::string response;
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string url = "https://vision.googleapis.com/v1/images:annotate?key=" + apiKey;
        std::string requestData = requestBody.dump();

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, requestData.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    try {
        auto jsonResponse = json::parse(response);
        return jsonResponse["responses"][0]["fullTextAnnotation"]["text"];
    }
    catch (...) {
        return "";
    }
}





int main() {
    SetConsoleOutputCP(CP_UTF8);  
    SetConsoleCP(CP_UTF8);


    std::string imagePath;
    std::cout << " Enter the image path: "; //you can type test,test1,test2.png to try out first, they are in the folder
    std::getline(std::cin, imagePath);

    cv::Mat img = cv::imread(imagePath);
    if (img.empty()) {
        std::cerr << "can't read the photo, Please check Path¡£\n";
        return 1;
    }
    //pre-processing
    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);           // Grayscale
    cv::GaussianBlur(img, img, cv::Size(5, 5), 0);        // Denoising
    cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);  // Binarization
    cv::imwrite("processed.png", img);

  


    /* test with photos in projrct file
   Pix* image = pixRead(R"(test.png)");
   if (!image) {
       std::cerr << "can't read the photo, Please check Path¡£\n";
       return 1;
   }
   */

   //OCR API
    std::string ocrResult = performOCRWithGoogleVision("processed.png");
    
    std::wcout.imbue(std::locale("chs"));
    std::wcout << L"\n OCR result£º\n" << utf8ToWstring(ocrResult) << std::endl;

    // translation
    std::string translated = translateText(ocrResult, "en");
    std::wcout << L"\n translation result£º\n" << utf8ToWstring(translated) << std::endl;

    std::ofstream outFile("output.txt", std::ios::out | std::ios::binary);
    outFile << "OCR Result:\n" << ocrResult << "\n\n";
    outFile << "Translation Result:\n" << translated << std::endl;

    return 0;
}
