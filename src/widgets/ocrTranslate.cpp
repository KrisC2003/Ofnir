// ocr_translate.cpp
#include "ocrTranslate.h"

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::wstring utf8ToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

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

std::string translateText(const std::string& text, const std::string& targetLang) {
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
        return htmlEntityDecode(jsonResponse["data"]["translations"][0]["translatedText"]);
    }
    catch (...) {
        return "";
    }
}

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

std::string performOCRWithGoogleVision(const std::string& imagePath, float confidenceLevel) {
    std::string apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  // Api Key
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

    std::string resultText;

    try {
        auto jsonResponse = json::parse(response);
        auto pages = jsonResponse["responses"][0]["fullTextAnnotation"]["pages"];
        for (const auto& page : pages) {
            for (const auto& block : page["blocks"]) {
                for (const auto& paragraph : block["paragraphs"]) {
                    for (const auto& word : paragraph["words"]) {
                        float conf = word.value("confidence", 1.0f);
                        if (conf >= confidenceLevel) {
                            std::string wordText;
                            for (const auto& symbol : word["symbols"]) {
                                wordText += symbol["text"].get<std::string>();
                            }
                            resultText += wordText + " ";
                        }
                    }
                    resultText += "\n";
                }
            }
        }
    }
    catch (...) {
        return "";
    }

    return resultText;
}