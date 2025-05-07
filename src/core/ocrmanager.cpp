// ocr_translate.cpp
#include "ocrmanager.h"

OCRManager::OCRManager(QObject* parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
    netInfo = QNetworkInformation::instance();
}

// retries until maxRetries or confidence level is met
// TODO: future remake for compatibility with languages other than <select language> -> english
QString OCRManager::processOCRWithConfidence(const QString& imagePath) {
    cv::Mat img = loadImage(imagePath);
    imgProcessor.initPreprocessImg(img);

    //test file
    cv::Mat resultFiltered = img.clone();
    cv::imwrite("processed_img.png", resultFiltered);

    int maxRetries = 2;
    int retryCount = 0;
    QString resultText;

    while (retryCount < maxRetries) {
        try {
            QJsonDocument jsonDoc = fetchOCRResponse(imagePath);
            QJsonObject jsonResponse = jsonDoc.object();
            QJsonArray responses = jsonResponse["responses"].toArray();

            if (responses.isEmpty() || !responses[0].toObject().contains("fullTextAnnotation")) {
                qWarning() << "No text detected.";
                return "No text detected";
            }

            QJsonObject annotation = responses[0].toObject()["fullTextAnnotation"].toObject();
            QString text = annotation["text"].toString();
            if (text.isEmpty()) {
                qWarning() << "Empty text field in annotation.\n";
                return "No text detected";
            }
            QJsonArray pages = annotation["pages"].toArray();;
            float lowestBlockConfidence = 1.0f;
            for (const QJsonValue& pageVal : pages) {
                QJsonObject page = pageVal.toObject();
                QJsonArray blocks = page["blocks"].toArray();

                for (const QJsonValue& blockVal : blocks) {
                    QJsonObject block = blockVal.toObject();
                    float blockConfidence = block.value("confidence").toDouble(1.0f);

                    if (blockConfidence < lowestBlockConfidence) {
                        lowestBlockConfidence = blockConfidence;
                    }

                    QJsonArray paragraphs = block["paragraphs"].toArray();
                    for (const QJsonValue& paragraphVal : paragraphs) {
                        QJsonObject paragraph = paragraphVal.toObject();
                        QJsonArray words = paragraph["words"].toArray();

                        for (const QJsonValue& wordVal : words) {
                            QJsonObject word = wordVal.toObject();
                            QJsonArray symbols = word["symbols"].toArray();

                            QString wordText;

                            for (const QJsonValue& symbolVal : symbols) {
                                QJsonObject symbol = symbolVal.toObject();
                                wordText += symbol["text"].toString();
                            }
                            resultText += wordText + " ";

                        }
                        resultText += "\n";
                    }
                }
            }
            bool retry = imgProcessor.preprocessImg(img, lowestBlockConfidence);

            if (!retry) {
                break;
            }
        }
        catch (const QJsonParseError& e) {
            qWarning() << "Error parsing JSON response: " << e.errorString();
            return "JSON Parsing Error";
        }
        catch (const std::exception& e) {
            qWarning() << "Error during OCR processing: " << e.what();
            return "OCR Processing Error";
        }
        catch (...) {
            std::cerr << "Unknown error occurred.\n";
            return "Unknown Error";
        }
        retryCount++;
    }
    if (retryCount == maxRetries) {
        qWarning() << "Max retries reached";
    }
    return resultText;
}

// translates text 
QString OCRManager::translateText(const QString& text, const QString& targetLang) {
    QString response;
    QNetworkRequest request;

    QString url = "https://translation.googleapis.com/language/translate/v2?key=" + apiKey +
        "&q=" + QUrl::toPercentEncoding(text) + "&target=" + targetLang;

    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QEventLoop eventLoop;
    connect(networkManager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);

    QNetworkReply* reply = networkManager->get(request);

    //prevents infinite hang due to network issues 
    // TODO: pass to error handler to give message in this scenario
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    timer.start(5000);

    eventLoop.exec();

    QString translatedText;
    if (checkNetworkStatus()) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        if (!jsonResponse.isNull()) {
            QJsonObject responseObject = jsonResponse.object();
            if (responseObject.contains("data")) {
                QJsonObject data = responseObject["data"].toObject();
                if (data.contains("translations")) {
                    QJsonArray translations = data["translations"].toArray();
                    if (!translations.isEmpty()) {
                        QJsonObject translation = translations[0].toObject();
                        translatedText = translation["translatedText"].toString();
                    }
                }
            }
        }
    }
    reply->deleteLater();
    return translatedText;
}

QJsonDocument OCRManager::fetchOCRResponse(const QString& imagePath) {
    QNetworkRequest request;
    QString base64Image = encodeImageToBase64(imagePath);
    if (base64Image.isEmpty()) {
        qWarning() << "Failed to encode image.";
        return QJsonDocument();
    }

    QJsonObject requestBody = {
        { "requests", QJsonArray{
            QJsonObject{
                { "image", QJsonObject{
                    { "content", base64Image }
                }},
                { "features", QJsonArray{
                    QJsonObject{ { "type", "TEXT_DETECTION" } }
                }}
            }
        }}
    };

    QString url = QString("https://vision.googleapis.com/v1/images:annotate?key=%1").arg(apiKey);
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QEventLoop eventLoop;

    QJsonDocument requestData(requestBody);

    QNetworkReply* reply = networkManager->post(request, requestData.toJson());

    connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);

    //prevents infinite hang due to network issues 
    // TODO: pass to error handler to give message in this scenario
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
    timer.start(5000);

    eventLoop.exec();

    QJsonDocument jsonResponse;
    if (checkNetworkStatus() && reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        jsonResponse = QJsonDocument::fromJson(responseData);
    }
    else {
        qWarning() << "Network error or status check failed:" << reply->errorString();
    }
    reply->deleteLater();
    return jsonResponse;
}

cv::Mat OCRManager::loadImage(const QString& imagePath) {
    std::string stringPath = imagePath.toStdString();
    cv::Mat img = cv::imread(stringPath);
    if (img.empty()) {
        throw std::runtime_error("Cannot read the image.");
    }
    return img;
}

// converts non-ASCII special characters for display
std::wstring OCRManager::convertMultilangUTF8ToWstring(const QString& qstr) {
    std::wstring wstr = qstr.toStdWString();
    return wstr;
}

// converts htmlEntities to be more readable characters (for future use as google vision doesnt return htmlEntities)
QString OCRManager::htmlEntityDecode(const QString& input) {
    QHash<QString, QChar> html_entities = {
        {"&quot;", '\"'}, {"&apos;", '\''}, {"&amp;", '&'},
        {"&lt;", '<'}, {"&gt;", '>'}, {"&#39;", '\''}
    };

    QString output = input;
    for (const auto& entity : html_entities) {
        output.replace(entity, html_entities[entity]);
    }
    return output;
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

// not used anymore?
size_t OCRManager::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

bool OCRManager::checkNetworkStatus() {
    QNetworkInformation::Reachability status = netInfo->reachability();

    switch (status) {
    case QNetworkInformation::Reachability::Online:
        qDebug() << "Network is online.";
        return true;
    case QNetworkInformation::Reachability::Site:
        qDebug() << "Can reach specific sites.";
        return true;
    case QNetworkInformation::Reachability::Local:
        qDebug() << "Connected locally only.";
        return false;
    case QNetworkInformation::Reachability::Disconnected:
        qDebug() << "No network connection.";
        return false;
    default:
        qDebug() << "Unknown network status.";
        return false;
    }
}

bool OCRManager::isCJLanguage(const QString& language) {
    return language == "zh" || language == "ja";
}