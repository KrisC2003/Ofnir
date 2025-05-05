#include "QtWidgetsApplication1.h"
#include "FontSettingsDialog.h"
#include "ScreenshotOverlay.h"
#include "OCRUtil.h"
#include "VisualComponents.h"

#include <QCheckBox>
#include <opencv2/opencv.hpp>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QClipboard>
#include <QGuiApplication>
#include <QScreen>
#include <QFontMetrics>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QHeaderView>
#include <QDateTime>
#include <QMessageBox>
#include <QTableWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QDialog>
#include <QTextStream>
#include <QStackedLayout>

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

    // Create hidden history table (data storage)
    m_historyTable = new QTableWidget(); // no parent


    // Settings button
    QPushButton* settingBtn = new QPushButton("Settings", this);
    settingBtn->setGeometry(10, 10, 100, 30);
    connect(settingBtn, &QPushButton::clicked, this, [this]() {
        FontSettingsDialog dlg(this);
        if (dlg.exec() == QDialog::Accepted) {
            QFont dummyFont;
            QColor dummyText, dummyOutline;
            QKeySequence newHotkey;
            QString dummyLang;
            loadSettingsFromJson("user_settings.json", dummyFont, dummyText, dummyOutline, newHotkey, dummyLang);
            if (m_hotkey) m_hotkey->setRegistered(false);
            m_hotkey->setShortcut(newHotkey, true);
        }
        });

    // History button
    QPushButton* historyBtn = new QPushButton("History", this);
    historyBtn->setGeometry(120, 10, 100, 30);
    connect(historyBtn, &QPushButton::clicked, this, &QtWidgetsApplication1::showHistoryDialog);

    setWindowTitle("OCR Translator");
    resize(250, 100);
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

            // ⬇️ Save history to JSON
            QFile file("translation_history.json");
            QJsonArray historyArray;
            if (file.open(QIODevice::ReadOnly)) {
                QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
                if (doc.isArray()) historyArray = doc.array();
                file.close();
            }
            QJsonObject entry;
            entry["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
            entry["source"] = QString::fromStdString(ocrResult);
            entry["translated"] = display;
            entry["language"] = language;
            historyArray.append(entry);
            if (file.open(QIODevice::WriteOnly)) {
                QJsonDocument doc(historyArray);
                file.write(doc.toJson(QJsonDocument::Indented));
                file.close();
            }
        });
}



void QtWidgetsApplication1::showHistoryDialog() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Translation History");
    dialog->resize(800, 500);

    QVBoxLayout* dialogLayout = new QVBoxLayout(dialog);

    // Create table
    QTableWidget* dialogTable = new QTableWidget(dialog);
    dialogTable->setColumnCount(7);
    dialogTable->setHorizontalHeaderLabels({ "", "Time", "Source", "Translated", "Language", "Copy", "Delete" });
    dialogTable->horizontalHeader()->setStretchLastSection(true);
    dialogLayout->addWidget(dialogTable);

    // --- Load JSON ---
    QFile jsonFile("translation_history.json");
    if (jsonFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = jsonFile.readAll();
        jsonFile.close();

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            for (int i = 0; i < arr.size(); ++i) {
                QJsonObject obj = arr.at(i).toObject();
                dialogTable->insertRow(i);

                // Checkbox
                QCheckBox* checkBox = new QCheckBox(dialogTable);
                dialogTable->setCellWidget(i, 0, checkBox);

                // Fill columns
                dialogTable->setItem(i, 1, new QTableWidgetItem(obj.value("time").toString()));
                dialogTable->setItem(i, 2, new QTableWidgetItem(obj.value("source").toString()));
                dialogTable->setItem(i, 3, new QTableWidgetItem(obj.value("translated").toString()));
                dialogTable->setItem(i, 4, new QTableWidgetItem(obj.value("language").toString()));

                // Delete button
                QPushButton* deleteBtn = new QPushButton("❌", dialogTable);
                deleteBtn->setProperty("row", i);
                dialogTable->setCellWidget(i, 6, deleteBtn);

      
                connect(deleteBtn, &QPushButton::clicked, this, [this, dialogTable, deleteBtn]() {
                    int row = deleteBtn->property("row").toInt();
                    dialogTable->removeRow(row);

                    
                    for (int r = 0; r < dialogTable->rowCount(); ++r) {
                        QWidget* cellWidget = dialogTable->cellWidget(r, 6);
                        if (cellWidget) {
                            QPushButton* btn = qobject_cast<QPushButton*>(cellWidget);
                            if (btn) btn->setProperty("row", r);
                        }
                    }

       
                    QJsonArray newArray;
                    for (int r = 0; r < dialogTable->rowCount(); ++r) {
                        QJsonObject obj;
                        obj["time"] = dialogTable->item(r, 1)->text();
                        obj["source"] = dialogTable->item(r, 2)->text();
                        obj["translated"] = dialogTable->item(r, 3)->text();
                        obj["language"] = dialogTable->item(r, 4)->text();
                        newArray.append(obj);
                    }
                    QFile file("translation_history.json");
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QJsonDocument doc(newArray);
                        file.write(doc.toJson(QJsonDocument::Indented));
                        file.close();
                    }
                    });

                // Copy button + label
                QWidget* copyCell = new QWidget(dialogTable);
                QHBoxLayout* copyLayout = new QHBoxLayout(copyCell);
                copyLayout->setContentsMargins(0, 0, 0, 0);

                QPushButton* copyBtn = new QPushButton("Copy", copyCell);
                QLabel* copiedLabel = new QLabel("Copied!", copyCell);
                copiedLabel->setStyleSheet("color: green; font-size: 10px;");
                copiedLabel->setVisible(false);

                copyLayout->addWidget(copyBtn);
                copyLayout->addWidget(copiedLabel);

                dialogTable->setCellWidget(i, 5, copyCell);

               
                connect(copyBtn, &QPushButton::clicked, this, [dialogTable, i, copiedLabel]() {
                    QClipboard* clipboard = QGuiApplication::clipboard();
                    clipboard->setText(dialogTable->item(i, 3)->text());

                    copiedLabel->setVisible(true);
                    copiedLabel->setWindowOpacity(1.0);

                    QPropertyAnimation* fade = new QPropertyAnimation(copiedLabel, "windowOpacity");
                    fade->setDuration(2000);
                    fade->setStartValue(1.0);
                    fade->setEndValue(0.0);
                    fade->start(QAbstractAnimation::DeleteWhenStopped);
                    QObject::connect(fade, &QPropertyAnimation::finished, copiedLabel, [copiedLabel]() {
                        copiedLabel->setVisible(false);
                        });
                    
                    });
                
            }
        }
    }

    // Export button
    QPushButton* exportBtn = new QPushButton("Export Selected", dialog);
    dialogLayout->addWidget(exportBtn);

    // Export feedback label
    QLabel* exportLabel = new QLabel("Exported!", dialog);
    exportLabel->setStyleSheet("color: blue; font-size: 12px;");
    exportLabel->setAlignment(Qt::AlignCenter);
    exportLabel->setVisible(false);
    dialogLayout->addWidget(exportLabel);

    connect(exportBtn, &QPushButton::clicked, this, [dialogTable, exportLabel]() {
        QFile outFile("history_export.txt");
        if (outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&outFile);
            for (int row = 0; row < dialogTable->rowCount(); ++row) {
                QCheckBox* check = qobject_cast<QCheckBox*>(dialogTable->cellWidget(row, 0));
                if (check && check->isChecked()) {
                    QString source = dialogTable->item(row, 2)->text();
                    QString translated = dialogTable->item(row, 3)->text();
                    out << "\t" << source.replace("\n", " ") << "\n" << translated.replace("\n", " ") << "\n";
                }
            }
            outFile.close();

            exportLabel->setVisible(true);
            exportLabel->setWindowOpacity(1.0);

            QPropertyAnimation* fade = new QPropertyAnimation(exportLabel, "windowOpacity");
            fade->setDuration(2000);
            fade->setStartValue(1.0);
            fade->setEndValue(0.0);
            fade->start(QAbstractAnimation::DeleteWhenStopped);
            QObject::connect(fade, &QPropertyAnimation::finished, exportLabel, [exportLabel]() {
                exportLabel->setVisible(false);
                });
        }
        });
    dialog->exec();

 
}


QWidget* QtWidgetsApplication1::createResultPopup(const QString& text, const QRect& anchorRect)
{
    QFont font;
    QColor textColor, outlineColor;
    QKeySequence dummy;
    QString dummyLang;
    loadSettingsFromJson("user_settings.json", font, textColor, outlineColor, dummy, dummyLang);

    DraggablePopup* popup = new DraggablePopup();
    popup->setStyleSheet("background: transparent; border: none;");

    QVBoxLayout* outerLayout = new QVBoxLayout(popup);
    outerLayout->setContentsMargins(15, 15, 15, 15);

    QWidget* content = new QWidget(popup);
    content->setStyleSheet("background-color: rgba(255,255,255,60); border: 1px solid rgba(120,120,120,0.6); border-radius: 10px;");
    QVBoxLayout* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(10, 10, 10, 10);

    QPushButton* closeButton = new QPushButton("\u274C", content);
    closeButton->setFixedSize(28, 28);
    connect(closeButton, &QPushButton::clicked, popup, &QWidget::close);

    QPushButton* copyButton = new QPushButton(QString::fromUtf8("\xF0\x9F\x93\x8B"), content);
    copyButton->setToolTip("Copy to clipboard");
    copyButton->setFixedSize(28, 28);

    OutlinedLabel* label = new OutlinedLabel(content);
    label->setText(text);
    label->setFont(font);
    label->setTextColor(textColor);
    label->setOutlineColor(outlineColor);
    label->setWordWrap(true);

    QFontMetrics metrics(font);
    int maxWidth = 650;
    int height = metrics.lineSpacing() * (text.count('\n') + 1) + 30;

    label->setMinimumWidth(maxWidth);
    label->setFixedSize(maxWidth+200, height);


    QLabel* copiedLabel = new QLabel("Copied!", content);
    copiedLabel->setAlignment(Qt::AlignCenter);
    copiedLabel->setStyleSheet("background: rgba(0,0,0,0.7); color: white; padding: 5px; border-radius: 5px;");
    copiedLabel->setVisible(false);


    QStackedLayout* stackedLayout = new QStackedLayout();
    stackedLayout->addWidget(label);
    stackedLayout->addWidget(copiedLabel);
    contentLayout->addLayout(stackedLayout);


    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(copyButton);
    topLayout->addWidget(closeButton);
    contentLayout->insertLayout(0, topLayout);  


    content->setFixedSize(maxWidth + 200, height + 30);
    outerLayout->addWidget(content);
    popup->setFixedSize(maxWidth + 220, height + 60);

 
    connect(copyButton, &QPushButton::clicked, [text, copiedLabel]() {
        QClipboard* clipboard = QGuiApplication::clipboard();
        clipboard->setText(QString(text).replace("\n", " "));

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

    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    int padding = 10;
    QPoint targetPos(
        anchorRect.right() + padding,
        anchorRect.top() + (anchorRect.height() - popup->height()) / 2
    );
    if (targetPos.x() + popup->width() > screenGeometry.right())
        targetPos.setX(anchorRect.left() - popup->width() - padding);
    if (targetPos.y() < padding)
        targetPos.setY(padding);
    else if (targetPos.y() + popup->height() > screenGeometry.bottom())
        targetPos.setY(screenGeometry.bottom() - popup->height() - padding);

    popup->move(targetPos);
    popup->show();

    return popup;
}

