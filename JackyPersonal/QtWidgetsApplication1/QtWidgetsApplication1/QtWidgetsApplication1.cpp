#include "QtWidgetsApplication1.h"
#include "ScreenshotOverlay.h"
#include "QTCV.h"
#include "OcrAndTranslate.h"
#include "HotkeyFilter.h"
#include <opencv2/opencv.hpp>
#include <QFile>
#include <QDebug>
#include <fstream>
#include <QScreen>
#include <QGuiApplication>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <QVBoxLayout>
#include <QPushButton>
#include "OutlinedLabel.h"
#include <QRegularExpression>
#include <QFontMetrics>
#include <QTextDocument>  
#include <QPropertyAnimation>
#include "DraggablePopup.h"


#define HOTKEY_ID 1001

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // 
   // textOutput = new QTextEdit(this);
    //textOutput->setGeometry(820, 10, 300, 580);
    //textOutput->setReadOnly(true);
    //textOutput->setStyleSheet("font-size: 16px; background-color: #f0f0f0; color: #222;");


    
    RegisterHotKey((HWND)this->winId(), HOTKEY_ID, MOD_ALT, 0x58); // alt +X 

   
    HotkeyFilter* filter = new HotkeyFilter(this);
    qApp->installNativeEventFilter(filter);
    connect(filter, &HotkeyFilter::hotkeyPressed, this, &QtWidgetsApplication1::captureAndShowScreenshot);
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
    UnregisterHotKey((HWND)this->winId(), HOTKEY_ID);
}

void QtWidgetsApplication1::captureAndShowScreenshot()
{
    ScreenshotOverlay* overlay = new ScreenshotOverlay();
    connect(overlay, &ScreenshotOverlay::screenshotTakenWithRect, this,
        [this](const QPixmap& pixmap, const QRect& rect) {

            // image preprocess
            cv::Mat img = QPixmapToMat(pixmap);
            cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
            cv::GaussianBlur(img, img, cv::Size(5, 5), 0);
            cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
            cv::imwrite("processed.png", img);

            // OCR and translate
            std::string ocrResult = performOCRWithGoogleVision("processed.png", 0.9f);
            std::wstring ocrText = utf8ToWstring(ocrResult);
            std::string translated = translateText(ocrResult, "en");
            std::wstring translatedText = utf8ToWstring(translated);

            // to words
            QString display = QString::fromStdWString(
               translatedText
            );

            
            createResultPopup(display, rect);

            //  write in file
            //std::ofstream outFile("output.txt", std::ios::out | std::ios::binary);
            //outFile << "OCR Result:\n" << ocrResult << "\n\n";
            //outFile << "Translation Result:\n" << translated << std::endl;
        });
}



QWidget* QtWidgetsApplication1::createResultPopup(const QString& text, const QRect& anchorRect)
{
    DraggablePopup* popup = new DraggablePopup();

    popup->setStyleSheet("background-color: rgba(255, 255, 255, 230); border: 2px solid black;");
    QVBoxLayout* layout = new QVBoxLayout(popup);
    layout->setContentsMargins(10, 10, 10, 10);

    // close button
    QPushButton* closeButton = new QPushButton("❌", popup);
    closeButton->setStyleSheet(
        "QPushButton { color: black; background-color: transparent; border: none; font-size: 16px; }"
        "QPushButton:hover { color: red; }"
    );
    closeButton->setFixedSize(28, 28);
    closeButton->setCursor(Qt::PointingHandCursor);
    connect(closeButton, &QPushButton::clicked, popup, &QWidget::close);

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(closeButton);
    layout->addLayout(topLayout);

    // show 
    OutlinedLabel* label = new OutlinedLabel(popup);
    label->setText(text);
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    label->setWordWrap(true);
    label->setFixedSize(600, 600);
    layout->addWidget(label);

    popup->setFixedSize(620, 620);

    // paraelle
    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int padding = 10;

    // default right side
    QPoint targetPos = QPoint(
        anchorRect.right() + padding,
        anchorRect.top() + (anchorRect.height() - popup->height()) / 2
    );

    // show at left if not enough space
    if (targetPos.x() + popup->width() > screenGeometry.right()) {
        targetPos.setX(anchorRect.left() - popup->width() - padding);
    }

    // show higher or lower if not enough space
    if (targetPos.y() < padding) {
        targetPos.setY(padding);
    }
    else if (targetPos.y() + popup->height() > screenGeometry.bottom()) {
        targetPos.setY(screenGeometry.bottom() - popup->height() - padding);
    }

    popup->move(targetPos);

    
    popup->setWindowOpacity(0.0);
    popup->show();

    QPropertyAnimation* animation = new QPropertyAnimation(popup, "windowOpacity");
    animation->setDuration(200);  // 
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    
    return popup;
}