#include "QtWidgetsApplication1.h"
#include "FontSettingsDialog.h"
#include "ScreenshotOverlay.h"
#include "OCRUtil.h"
#include "VisualComponents.h"  
#include <opencv2/opencv.hpp>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QClipboard>
#include <QGuiApplication>
#include <QScreen>
#include <QFontMetrics>

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
    : QDialog(parent), m_hotkey(nullptr)
{
    // Load settings
    QFont dummyFont;
    QColor dummyText, dummyOutline;
    QKeySequence loadedHotkey;
    QString language;
    loadSettingsFromJson("user_settings.json", dummyFont, dummyText, dummyOutline, loadedHotkey, language);

    // Register hotkey
    m_hotkey = new QHotkey(loadedHotkey, true, this);
    connect(m_hotkey, &QHotkey::activated, this, &QtWidgetsApplication1::captureAndShowScreenshot);

    // Settings button
    QPushButton* settingBtn = new QPushButton("Settings", this);
    settingBtn->setGeometry(10, 10, 100, 30);
    connect(settingBtn, &QPushButton::clicked, this, [this]() {
        FontSettingsDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            QFont dummyFont;
            QColor dummyText, dummyOutline;
            QKeySequence newHotkey;
            QString dummyLanguage;
            loadSettingsFromJson("user_settings.json", dummyFont, dummyText, dummyOutline, newHotkey, dummyLanguage);
            if (m_hotkey) m_hotkey->setRegistered(false);
            m_hotkey->setShortcut(newHotkey, true);
        }
        });

    setWindowTitle("OCR Translator");
    resize(200, 100);
}

QtWidgetsApplication1::~QtWidgetsApplication1()
{
    if (m_hotkey) m_hotkey->setRegistered(false);
}

void QtWidgetsApplication1::captureAndShowScreenshot()
{
    ScreenshotOverlay* overlay = new ScreenshotOverlay();
    connect(overlay, &ScreenshotOverlay::screenshotTakenWithRect, this,
        [this](const QPixmap& pixmap, const QRect& rect) {
            cv::Mat img = QPixmapToMat(pixmap);
            cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
            cv::GaussianBlur(img, img, cv::Size(5, 5), 0);
            cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
            cv::imwrite("processed.png", img);

            QFont dummyFont;
            QColor dummyText, dummyOutline;
            QKeySequence dummyHotkey;
            QString language;
            loadSettingsFromJson("user_settings.json", dummyFont, dummyText, dummyOutline, dummyHotkey, language);

            std::string ocrResult = performOCRWithGoogleVision("processed.png", 0.9f);
            std::wstring translatedText = utf8ToWstring(translateText(ocrResult, language.toStdString()));
            QString display = QString::fromStdWString(translatedText);

            createResultPopup(display, rect);
        });
}

QWidget* QtWidgetsApplication1::createResultPopup(const QString& text, const QRect& anchorRect)
{
    QFont font;
    QColor textColor, outlineColor;
    QKeySequence dummy;
    QString dummyLang;
    loadSettingsFromJson("user_settings.json", font, textColor, outlineColor, dummy, dummyLang);

    DraggablePopup* popup = new DraggablePopup();
    popup->setStyleSheet("background-color: rgba(255, 255, 255, 230); border: 2px solid black;");

    QVBoxLayout* layout = new QVBoxLayout(popup);
    layout->setContentsMargins(10, 10, 10, 10);

    QPushButton* closeButton = new QPushButton("\u274C", popup);
    closeButton->setStyleSheet("QPushButton { color: black; background-color: transparent; border: none; font-size: 16px; } QPushButton:hover { color: red; }");
    closeButton->setFixedSize(28, 28);
    connect(closeButton, &QPushButton::clicked, popup, &QWidget::close);

    QPushButton* copyButton = new QPushButton(QString::fromUtf8("\xF0\x9F\x93\x8B"), popup);
    copyButton->setToolTip("Copy to clipboard");
    copyButton->setStyleSheet("QPushButton { color: black; background-color: transparent; border: none; font-size: 16px; } QPushButton:hover { color: green; }");
    copyButton->setFixedSize(28, 28);

    QLabel* copiedLabel = new QLabel("Copied to clipboard", popup);
    copiedLabel->setStyleSheet("color: green; background: transparent; font-size: 12px;");
    copiedLabel->setVisible(false);
    layout->addWidget(copiedLabel);

    connect(copyButton, &QPushButton::clicked, [text, copiedLabel]() {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(text);
        copiedLabel->setVisible(true);
        copiedLabel->setWindowOpacity(1.0);

        QPropertyAnimation* fade = new QPropertyAnimation(copiedLabel, "windowOpacity");
        fade->setDuration(1500);
        fade->setStartValue(1.0);
        fade->setEndValue(0.0);
        fade->start(QAbstractAnimation::DeleteWhenStopped);

        QObject::connect(fade, &QPropertyAnimation::finished, copiedLabel, [copiedLabel]() {
            copiedLabel->setVisible(false);
            });
        });

    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(copyButton);
    topLayout->addWidget(closeButton);
    layout->addLayout(topLayout);

    OutlinedLabel* label = new OutlinedLabel(popup);
    label->setText(text);
    label->setFont(font);
    label->setTextColor(textColor);
    label->setOutlineColor(outlineColor);
    label->setWordWrap(true);

    QFontMetrics metrics(font);
    int maxWidth = 650;
    int lineSpacing = metrics.lineSpacing();
    QStringList lines = text.split('\n');
    int height = lines.count() * lineSpacing + 30;

    label->setMinimumWidth(maxWidth);
    label->setFixedSize(maxWidth + 150, height);
    layout->addWidget(label);

    popup->setFixedSize(maxWidth + 150, height + 30);

    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int padding = 10;
    QPoint targetPos = QPoint(
        anchorRect.right() + padding,
        anchorRect.top() + (anchorRect.height() - popup->height()) / 2
    );

    if (targetPos.x() + popup->width() > screenGeometry.right()) {
        targetPos.setX(anchorRect.left() - popup->width() - padding);
    }
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
    animation->setDuration(200);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    return popup;
}
