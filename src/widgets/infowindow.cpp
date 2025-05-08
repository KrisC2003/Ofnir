#include "infowindow.h"
#include "ui_infowindow.h"
#include "filelistwidget.h"
#include <QIcon>
#include "src/settings/globalHotkeyFilter.h"
#include "screenCaptureWidget.h"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidget>
#include <QStringList>
#include <QColorDialog>
#include <QPalette>
#include <QFontDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QMessageBox>
#include <QJsonArray>
#define HOTKEY_ID 1001

InfoWindow::InfoWindow(QDialog* parent)
    : QDialog(parent), ui(new Ui::InfoWindow)
{

    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon.png"));
    ui->importButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #dedede;"
        "  border: none;"
        "}"
        "QPushButton:hover {"
        "  background-color: #898989;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #898989;"
        "}"
    );

    ui->exportButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #dedede;"
       
        "  border: none;"
       
        "}"
        "QPushButton:hover {"
        "  background-color: #898989;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #898989;"
        "}"
    );

    setWindowFlags(Qt::FramelessWindowHint);
    //connects ui buttons to functions
    connect(ui->closeButton, &QPushButton::clicked, this, &InfoWindow::close);
    connect(ui->colorButton, &QPushButton::clicked, this, &InfoWindow::changeBackgroundColor);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &InfoWindow::onTabChanged);
    connect(ui->fontStyle, &QPushButton::clicked, this, &InfoWindow::changeFont);
    connect(ui->importButton, &QPushButton::clicked, this, &InfoWindow::importFile);


    loadSettings();
    loadImportedFiles();

}

InfoWindow::~InfoWindow() {
    saveSettings();
    delete ui;
}

void InfoWindow::importFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "All Files (*.*)");

    if (!fileName.isEmpty()) {
        QFileInfo fileInfo(fileName);
        QString displayName = fileInfo.fileName();

        if (!m_importedFiles.contains(fileName)) {
            m_importedFiles.append(fileName);
            ui->fileListWidget->addItem(displayName);
            saveImportedFiles();  
        }
        else {
            QMessageBox::information(this, "Already Imported", "This file is already in the list.");
        }
    }
}

void InfoWindow::saveImportedFiles() {
    QJsonArray jsonArray;
    for (const QString& filePath : m_importedFiles) {
        jsonArray.append(filePath);
    }

    QJsonObject jsonObject;
    jsonObject["files"] = jsonArray;

    QJsonDocument doc(jsonObject);
    QString path = QDir::currentPath() + "/imported_files.json";
    QFile file(path);

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}


void InfoWindow::loadImportedFiles() {
    QString path = QDir::currentPath() + "/imported_files.json";
    QFile file(path);

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if (obj.contains("files")) {
            QJsonArray filesArray = obj["files"].toArray();
            for (const QJsonValue& value : filesArray) {
                QString filePath = value.toString();
                QFileInfo fileInfo(filePath);
                if (fileInfo.exists()) {
                    ui->fileListWidget->addItem(fileInfo.fileName());
                    m_importedFiles.append(filePath);
                }
            }
        }
    }
}




void InfoWindow::onTabChanged(int index)
{
    qDebug() << "Tab changed to Theme: " << index;
}

//CHANGE FONT BUTTON
void InfoWindow::changeFont() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);

    if (ok) {
        QColor color = QColorDialog::getColor(m_fontColor, this, "Select Font Color");

        if (color.isValid()) {
            m_fontColor = color;
            QString style = QString("color: %1; font-family: %2; font-size: %3pt; font-weight: %4; font-style: %5;")
                .arg(m_fontColor.name())   // Apply font color
                .arg(font.family())      // Apply font fmaily
                .arg(font.pointSize())   // Apply size
                .arg(font.weight() == QFont::Bold ? "bold" : "normal") // Apply bold
                .arg(font.italic() ? "italic" : "normal"); // Apply italic
            ui->tabWidget->setStyleSheet(style);
            ui->tabWidget->setFont(font);

            saveSettings();
        }
    }
}

// CHANGE BACKGROUND COLOR BUTTON
void InfoWindow::changeBackgroundColor() {
    QColor color = QColorDialog::getColor(m_currentColor, this, "Select Background Color");

    if (color.isValid()) {
        m_currentColor = color;
        QPalette palette;
        palette.setColor(QPalette::Window, m_currentColor);
        this->setAutoFillBackground(true);
        this->setPalette(palette);

        saveSettings();
    }
}

// Close event override as hideg
void InfoWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    this->hide();
}


// Mouse press events (For dragging)
void InfoWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && event->pos().y() <= m_dragAreaHeight) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        m_dragging = true;  // Enable dragging
        event->accept();
    }
    else {
        m_dragging = false;  // Disable dragging if clicked outside the draggable area
        event->ignore();
    }
}

void InfoWindow::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
    else {
        event->ignore();
    }
}
void InfoWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;  // Stop dragging when mouse is released
        event->accept();
    }
}

void InfoWindow::saveSettings() {
    QJsonObject settingsObj;
    settingsObj["backgroundColor"] = m_currentColor.name();
    settingsObj["fontColor"] = m_fontColor.name();
    settingsObj["fontFamily"] = ui->tabWidget->font().family();
    settingsObj["fontSize"] = ui->tabWidget->font().pointSize();
    settingsObj["fontBold"] = ui->tabWidget->font().bold();
    settingsObj["fontItalic"] = ui->tabWidget->font().italic();

    QJsonDocument doc(settingsObj);

    QString path = QDir::currentPath(); 
    QFile file(path + "/settings.json");

    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        
    }
}



void InfoWindow::loadSettings() {
    QString path = QDir::currentPath();  
    QFile file(path + "/settings.json");

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonObject obj = doc.object();

        if (obj.contains("backgroundColor"))
            m_currentColor = QColor(obj["backgroundColor"].toString());
        if (obj.contains("fontColor"))
            m_fontColor = QColor(obj["fontColor"].toString());

        QFont font;
        font.setFamily(obj["fontFamily"].toString());
        font.setPointSize(obj["fontSize"].toInt());
        font.setBold(obj["fontBold"].toBool());
        font.setItalic(obj["fontItalic"].toBool());

        QPalette palette;
        palette.setColor(QPalette::Window, m_currentColor);
        this->setAutoFillBackground(true);
        this->setPalette(palette);

        QString style = QString("color: %1; font-family: %2; font-size: %3pt; font-weight: %4; font-style: %5;")
            .arg(m_fontColor.name())
            .arg(font.family())
            .arg(font.pointSize())
            .arg(font.bold() ? "bold" : "normal")
            .arg(font.italic() ? "italic" : "normal");
        ui->tabWidget->setStyleSheet(style);
        ui->tabWidget->setFont(font);
    }
}






