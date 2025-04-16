#include "QtWidgetsApplication1.h"
#include "ScreenshotOverlay.h"
#include "OCRUtil.h"
#include "HotkeyFilter.h"
#include "FontSettingsDialog.h"
#include "VisualComponents.h"
#include <opencv2/opencv.hpp>
#include <QFile>
#include <QDebug>
#include <fstream>
#include <QScreen>
#include <QGuiApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QRegularExpression>
#include <QFontMetrics>
#include <QTextDocument>
#include <QPropertyAnimation>

#define HOTKEY_ID 1001

QtWidgetsApplication1::QtWidgetsApplication1(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // load setting
    QFont dummyFont;
    QColor dummyText, dummyOutline;
    QKeySequence loadedHotkey;
    loadSettingsFromJson("user_settings.json", dummyFont, dummyText, dummyOutline, loadedHotkey);

    QKeyCombination kc = loadedHotkey[0];
    int key = kc.key();
    int modifiers = 0;
    if (kc.keyboardModifiers().testFlag(Qt::ControlModifier)) modifiers |= MOD_CONTROL;
    if (kc.keyboardModifiers().testFlag(Qt::AltModifier))     modifiers |= MOD_ALT;
    if (kc.keyboardModifiers().testFlag(Qt::ShiftModifier))   modifiers |= MOD_SHIFT;
    RegisterHotKey((HWND)this->winId(), HOTKEY_ID, modifiers, key);

    // Install global hotkey event filter
    HotkeyFilter* filter = new HotkeyFilter(this);
    qApp->installNativeEventFilter(filter);
    connect(filter, &HotkeyFilter::hotkeyPressed, this, &QtWidgetsApplication1::captureAndShowScreenshot);

    // Settings button
    QPushButton* settingBtn = new QPushButton("Settings", this);
    settingBtn->setGeometry(10, 10, 100, 30);
    connect(settingBtn, &QPushButton::clicked, this, [this]() {
        FontSettingsDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            // Re-register hotkey
            QFont dummyFont;
            QColor dummyText, dummyOutline;
            QKeySequence newHotkey;
            loadSettingsFromJson("user_settings.json", dummyFont, dummyText, dummyOutline, newHotkey);

            UnregisterHotKey((HWND)this->winId(), HOTKEY_ID);
            QKeyCombination kc = newHotkey[0];
            int key = kc.key();
            int modifiers = 0;
            if (kc.keyboardModifiers().testFlag(Qt::ControlModifier)) modifiers |= MOD_CONTROL;
            if (kc.keyboardModifiers().testFlag(Qt::AltModifier))     modifiers |= MOD_ALT;
            if (kc.keyboardModifiers().testFlag(Qt::ShiftModifier))   modifiers |= MOD_SHIFT;
            RegisterHotKey((HWND)this->winId(), HOTKEY_ID, modifiers, key);
        }
        });
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
            // Image preprocessing
            cv::Mat img = QPixmapToMat(pixmap);
            cv::cvtColor(img, img, cv::COLOR_RGB2GRAY);
            cv::GaussianBlur(img, img, cv::Size(5, 5), 0);
            cv::threshold(img, img, 0, 255, cv::THRESH_BINARY + cv::THRESH_OTSU);
            cv::imwrite("processed.png", img);

            //  OCR and translation
            std::string ocrResult = performOCRWithGoogleVision("processed.png", 0.9f);
            std::wstring translatedText = utf8ToWstring(translateText(ocrResult, "en"));
            QString display = QString::fromStdWString(translatedText);

            createResultPopup(display, rect);
        });
}

QWidget* QtWidgetsApplication1::createResultPopup(const QString& text, const QRect& anchorRect)
{
    // read JSON
    QFont font;
    QColor textColor, outlineColor;
    QKeySequence dummy;
    loadSettingsFromJson("user_settings.json", font, textColor, outlineColor, dummy);

    DraggablePopup* popup = new DraggablePopup();
    popup->setStyleSheet("background-color: rgba(255, 255, 255, 230); border: 2px solid black;");

    QVBoxLayout* layout = new QVBoxLayout(popup);
    layout->setContentsMargins(10, 10, 10, 10);

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

    OutlinedLabel* label = new OutlinedLabel(popup);
    label->setText(text);
    label->setFont(font);
    label->setTextColor(textColor);
    label->setOutlineColor(outlineColor);
    label->setWordWrap(true);
    label->setFixedSize(650, 700);
    layout->addWidget(label);

    popup->setFixedSize(670, 720);

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
