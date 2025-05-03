// ocr_translate.cpp
#include "ocrTranslate.h"

size_t OCRManager::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// converts non-ASCII special characters for display
std::wstring OCRManager::convertMultilangUTF8ToWstring(const std::string& str) {
    // checks for size needed before converting and returning the wstring
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), nullptr, 0);
    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int)str.length(), &wstr[0], size_needed);
    return wstr;
}

// converts htmlEntities to be more readable characters (for future use as google vision doesnt return htmlEntities)
std::string OCRManager::htmlEntityDecode(const std::string& input) {
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

// translates text 
std::string OCRManager::translateText(const std::string& text, const std::string& targetLang) {
    std::string apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  // API Key
    CURL* curl;
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
        //return htmlEntityDecode(jsonResponse["data"]["translations"][0]["translatedText"]);
    }
    catch (...) {
        return "";
    }
}

std::string OCRManager::encodeImageToBase64(const std::string& imagePath) {
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

// sends 
std::string OCRManager::fetchOCRResponse(const std::string& imagePath) {
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

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

// retries until maxRetries or confidence level is met
// TODO: future remake for compatibility with languages other than <select language> -> english
std::string OCRManager::processOCRWithConfidence(const std::string& imagePath, float confidenceLevel) {
    cv::Mat img = cv::imread(imagePath);
    if (img.empty()) {
        std::cerr << "can't read the image, Please check path¡£\n";
        return "Image Load Error";
    }
    imgProcessor.initPreprocessImg(img);

    //test file
    cv::imwrite("processed.png", img);

    std::string resultText;

    int maxRetries = 2;
    int retryCount = 0;

    std::string response = fetchOCRResponse("processed.png");

    while (retryCount < maxRetries) {
        bool retry = false;
        try {
            auto jsonResponse = json::parse(response);
            auto pages = jsonResponse["responses"][0]["fullTextAnnotation"]["pages"];
            float lowestBlockConfidence = 1.0f;

            for (const auto& page : pages) {
                for (const auto& block : page["blocks"]) {
                    float blockConfidence = block.value("confidence", 1.0f);
                    if (blockConfidence < lowestBlockConfidence) {
                        lowestBlockConfidence = blockConfidence;
                    }
                    for (const auto& paragraph : block["paragraphs"]) {
                        for (const auto& word : paragraph["words"]) {
                            std::string wordText;
                            for (const auto& symbol : word["symbols"]) {
                                wordText += symbol["text"].get<std::string>();
                            }
                            resultText += wordText + " ";

                        }
                        resultText += "\n";
                    }
                }
            }
            retry = imgProcessor.preprocessImg(img, lowestBlockConfidence);
        }
        catch (const json::exception& e) {
            std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
            return "JSON Parsing Error";
        }
        catch (const std::exception& e) {
            std::cerr << "Error during OCR processing: " << e.what() << std::endl;
            return "OCR Processing Error";
        }
        catch (...) {
            std::cerr << "Unknown error occurred.\n";
            return "Unknown Error";
        }
        retryCount++;
    }
    if (retryCount == maxRetries) {
        std::cerr << "Max retries reached\n" << std::endl;
    }
    return resultText;
}