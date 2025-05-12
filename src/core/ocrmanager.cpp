#include "ocrmanager.h"

OCRManager::OCRManager(QObject* parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
}

// retries until maxRetries or confidence level is met
// TODO: future remake for compatibility with languages other than <select language> -> english
QVector<QPair<QString, QRect>> OCRManager::processOCRWithConfidence(const QString& imagePath) {
    cv::Mat img = loadImage(imagePath);
    imgProcessor.initPreprocessImg(img);

    // Test file
    //cv::Mat resultFiltered = img.clone();
    //cv::imwrite("processed_img.png", resultFiltered);

    int maxRetries = 2;
    int retryCount = 0;

    QByteArray responseData;
    if (!fetchOCRResponse(imagePath, responseData)) {
        qWarning() << "OCR request failed";
        return {};
    }

    // Parse and process the response data
    QJsonParseError parseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError || jsonResponse.isNull()) {
        qWarning() << "Failed to parse translation response:" << parseError.errorString();
        return {};
    }

    QJsonObject jsonResponseObj = jsonResponse.object();
    QJsonArray responses = jsonResponseObj["responses"].toArray();
    if (responses.isEmpty() || !responses[0].toObject().contains("fullTextAnnotation")) {
        qWarning() << "No text detected.";
        return {};
    }

    QJsonObject annotation = responses[0].toObject()["fullTextAnnotation"].toObject();
    QString text = annotation["text"].toString();
    if (text.isEmpty()) {
        qWarning() << "Empty text field in annotation.\n";
        return QVector<QPair<QString, QRect>>();
    }

    QJsonArray pages = annotation["pages"].toArray();
    float lowestBlockConfidence = 1.0f;

    QVector<QPair<QString, QRect>> allBlocks;

    for (const QJsonValue& pageVal : pages) {
        QJsonObject page = pageVal.toObject();
        QJsonArray blocks = page["blocks"].toArray();

        for (const QJsonValue& blockVal : blocks) {
            QJsonObject block = blockVal.toObject();
            float blockConfidence = block.value("confidence").toDouble(1.0f);
            if (blockConfidence < lowestBlockConfidence) {
                lowestBlockConfidence = blockConfidence;
            }

            QJsonObject boundingBoxObj = block["boundingBox"].toObject();
            QRect boundingBoxData = parseBoundingBox(boundingBoxObj);

            QJsonArray paragraphs = block["paragraphs"].toArray();
            for (const QJsonValue& paragraphVal : paragraphs) {
                QMap<QString, double> languageConfidenceMap;
                QString paragraphText;
                QJsonObject paragraph = paragraphVal.toObject();
                QJsonArray words = paragraph["words"].toArray();

                for (const QJsonValue& wordVal : words) {
                    QJsonObject word = wordVal.toObject();
                    QJsonArray symbols = word["symbols"].toArray();


                    for (const QJsonValue& symbolVal : symbols) {
                        QJsonObject symbol = symbolVal.toObject();
                        paragraphText += symbol["text"].toString();
                    }
                    // check whats the most dominant language and translate if needed essentially
                    if (word.contains("property")) {
                        QJsonObject property = word["property"].toObject();
                        if (property.contains("detectedLanguages")) {
                            QJsonArray languageDetected = property["detectedLanguages"].toArray();
                            for (const QJsonValue& langVal : languageDetected) {
                                QJsonObject language = langVal.toObject();
                                QString languageCode = language["languageCode"].toString();
                                double confidence = language["confidence"].toDouble();
                                languageConfidenceMap[languageCode] += confidence;
                            }
                        }
                    }
                    paragraphText = paragraphText.trimmed();

                    QJsonObject property = symbols.last().toObject()["property"].toObject();
                    QString breakType = property["detectedBreak"].toObject()["type"].toString();

                    if (breakType == "SPACE") paragraphText += " ";
                    else if (breakType == "EOL_SURE_SPACE" || breakType == "LINE_BREAK") paragraphText += "\n";
                    else if (breakType == "HYPHEN") paragraphText += "-";
                }

                if (!paragraphText.contains(QRegularExpression("\\p{L}"))) {
                    continue;
                }
                QString dominantLanguage;
                double maxConfidence = -1.0;
                for (auto it = languageConfidenceMap.begin(); it != languageConfidenceMap.end(); ++it) {
                    if (it.value() > maxConfidence) {
                        dominantLanguage = it.key();
                        maxConfidence = it.value();
                    }
                }

                if (dominantLanguage != "en") {
                    paragraphText = translateText(paragraphText, "en");
                }
                allBlocks.push_back(QPair<QString, QRect>(paragraphText, boundingBoxData));
            }
        }
    }

    // Retry logic based on OCR confidence
    while (retryCount < maxRetries) {
        bool retry = imgProcessor.preprocessImg(img, lowestBlockConfidence);
        if (!retry) {
            break;
        }

        if (!fetchOCRResponse(imagePath, responseData)) {
            qWarning() << "failed to fetch OCR response";
            return {};
        }

        retryCount++;
    }
    if (retryCount == maxRetries) {
        qWarning() << "Max retries reached";
    }

    return allBlocks;
}

QString OCRManager::translateText(const QString& text, const QString& targetLang) {
    if (text.isEmpty()) {
        qWarning() << "Input text is empty. (OCRManager::translateText)";
        return QString();
    }
    if (!checkNetworkStatus() || text.isEmpty()) {
        return QString();
    }

    QUrl url(QString("https://translation.googleapis.com/language/translate/v2"));
    QUrlQuery query;
    query.addQueryItem("key", apiKey);
    query.addQueryItem("q", text);
    query.addQueryItem("target", targetLang);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Set up event loop and timeout handling
    QEventLoop eventLoop;
    QTimer timer;
    QNetworkReply* reply = networkManager->get(request);    

    // TODO: pass to error handler to give message in this scenario
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &eventLoop, [&]() {
        reply->abort();
        eventLoop.quit();
        });

    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);

    timer.start(5000);  // 5 second timeout
    eventLoop.exec();

    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Translation request failed:" << reply->errorString();
        reply->deleteLater();
        return QString();
    }

    QByteArray responseData = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData, &parseError);
    if (parseError.error != QJsonParseError::NoError || jsonResponse.isNull()) {
        qWarning() << "Failed to parse translation response:" << parseError.errorString();
        return QString();
    }

    QString translatedText;
    QJsonObject responseObject = jsonResponse.object();
    QJsonObject data = responseObject["data"].toObject();
    QJsonArray translations = data["translations"].toArray();
    if (!translations.isEmpty()) {
        QJsonObject translation = translations[0].toObject();
        translatedText = translation["translatedText"].toString();

        // Check if the translated text is empty
        if (translatedText.isEmpty()) {
            qWarning() << "Translated text is empty.";
            translatedText = "No translation available";
        }
    }

    reply->deleteLater();
    translatedText = QTextDocumentFragment::fromHtml(translatedText).toPlainText(); //converts html entities
    return translatedText;
}

bool OCRManager::fetchOCRResponse(const QString& imagePath, QByteArray& responseData) {
    if (!checkNetworkStatus()) {
        qWarning() << "No network connection.";
        return false;
    }

    QString base64Image = encodeImageToBase64(imagePath);
    if (base64Image.isEmpty()) {
        qWarning() << "Failed to encode image.";
        return false;
    }

    QJsonObject requestBody = createOCRRequestBody(base64Image);

    QNetworkRequest request(QUrl("https://vision.googleapis.com/v1/images:annotate?key=" + apiKey));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    return sendNetworkRequest(request, QJsonDocument(requestBody).toJson(), responseData);
}

bool OCRManager::sendNetworkRequest(const QNetworkRequest& request, const QByteArray& data, QByteArray& responseData) {
    QNetworkReply* reply = networkManager->post(request, data);
    QEventLoop eventLoop;
    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        responseData = reply->readAll();
        reply->deleteLater();
        return true;
    }

    qWarning() << "Network request failed: " << reply->errorString();
    reply->deleteLater();
    return false;
}

QRect OCRManager::parseBoundingBox(const QJsonObject& boundingBoxObj) {
    QJsonArray verticesJson = boundingBoxObj["vertices"].toArray();
    int minX = INT_MAX, minY = INT_MAX, maxX = INT_MIN, maxY = INT_MIN;

    for (const QJsonValue& v : verticesJson) {
        QJsonObject point = v.toObject();
        int x = point["x"].toInt(0);
        int y = point["y"].toInt(0);
        minX = std::min(minX, x);
        minY = std::min(minY, y);
        maxX = std::max(maxX, x);
        maxY = std::max(maxY, y);
    }

    return QRect(minX, minY, maxX - minX, maxY - minY);
}

QJsonObject OCRManager::createOCRRequestBody(const QString& base64Image) {
    return QJsonObject{
        {"requests", QJsonArray{
            QJsonObject{
                {"image", QJsonObject{{"content", base64Image}}},
                {"features", QJsonArray{QJsonObject{{"type", "DOCUMENT_TEXT_DETECTION"}}}}
            }
        }}
    };
}

cv::Mat OCRManager::loadImage(const QString& imagePath) {
    std::string stringPath = imagePath.toStdString();
    cv::Mat img = cv::imread(stringPath);
    if (img.empty()) {
        throw std::runtime_error("Cannot read the image. (OCRManager::loadImage");
    }
    return img;
}

// converts non-ASCII special characters for display
std::wstring OCRManager::convertMultilangUTF8ToWstring(const QString& qstr) {
    std::wstring wstr = qstr.toStdWString();
    return wstr;
}

QString OCRManager::encodeImageToBase64(const QString& imagePath) {
    QFile file(imagePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open image:" << imagePath;
        return QString();
    }
    QByteArray imageData = file.readAll();
    return imageData.toBase64();
}

bool OCRManager::checkNetworkStatus() {
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://www.google.com"));
    QNetworkReply* reply = manager.get(request);

    // Wait for the request to finish
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    bool online = reply->error() == QNetworkReply::NoError;  // No error means the network is reachable
    reply->deleteLater();

    return online;
}

bool OCRManager::isCJLanguage(const QString& language) {
    return language == "zh" || language == "ja";
}