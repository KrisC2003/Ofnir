#pragma once


#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRect>
#include <QTimer>
#include "opencv2/opencv.hpp"
#include "imgpreprocessing.h" 

struct BlockData {
	QString text;
	QRect boundingBox;
};

class OCRManager : public QObject {
	Q_OBJECT
public:
	OCRManager(QObject* parent = nullptr);

	QVector<BlockData> processOCRWithConfidence(const QString& imagePath);
	QString translateText(const QString& text, const QString& targetLang);
	std::wstring convertMultilangUTF8ToWstring(const QString& str);

protected:
	QString htmlEntityDecode(const QString& input);

	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
	QString encodeImageToBase64(const QString& imagePath);
	bool fetchOCRResponse(const QString& imagePath, QByteArray& responseData);
private:
	QRect parseBoundingBox(const QJsonObject& boundingBoxObj);
	cv::Mat loadImage(const QString& imagePath);
	bool isCJLanguage(const QString& language);
	bool checkNetworkStatus();

	ImgPreprocessing imgProcessor;
	QNetworkAccessManager* networkManager;
	QString apiKey = "AIzaSyBKGpGr6xCOaISgDGoe-Vy_VAXK2nBWc9I";  // API Key

signals:
	void ocrResponseReceived(const QJsonDocument& response);
};


