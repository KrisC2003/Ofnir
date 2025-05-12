#include "src/core/ofnirdaemon.h"
#include "src/settings/settingsmanager.h"
#include "src/settings/globalhotkeyfilter.h"
#include "filelistwidget.h"
#include "infowindow.h"
#include "ui_infowindow.h"

InfoWindow::InfoWindow(OfnirDaemon* daemon, QDialog* parent)
    : QDialog(parent)
    , ui(new Ui::InfoWindow)
    , ofnirDaemon(daemon)
{

    ui->setupUi(this);

    connect(this, &InfoWindow::settingChanged, ofnirDaemon->settingsManager(), &SettingsManager::onSettingChanged);
    connect(this, &InfoWindow::multipleSettingsChanged, ofnirDaemon->settingsManager(), &SettingsManager::onSettingsChanged);
    connect(ofnirDaemon->settingsManager(), &SettingsManager::settingsLoaded, this, &InfoWindow::applySettings);
    ofnirDaemon->settingsManager()->load();

    setWindowIcon(QIcon(":/icon.png"));
    setWindowFlags(Qt::FramelessWindowHint);
    //connects ui buttons to functions

    connect(ofnirDaemon->hotkeyFilter(), &GlobalHotkeyFilter::hotkeyUpdated, this, [this](const QString& hotkeyText) {
            QLabel* label = ui->tabWidget->findChild<QLabel*>("currHotkey");
            label->setText(hotkeyText);
        });

    connect(ui->closeButton, &QPushButton::clicked, this, &InfoWindow::close);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &InfoWindow::onTabChanged);

    connect(ui->fontStyle, &QPushButton::clicked, this, &InfoWindow::changeFont);
    connect(ui->colorButton, &QPushButton::clicked, this, &InfoWindow::changeBackgroundColor);

    //connect(ui->importButton, &QPushButton::clicked, this, &InfoWindow::importFile);

    ofnirDaemon->hotkeyFilter()->registerShortcut();
    //loadSettings();
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

            QVariantMap innerFontSettings =
            {
                {"fontColor", m_fontColor.name()},
                {"fontFamily", ui->tabWidget->font().family()},
                {"fontSize", ui->tabWidget->font().pointSize()},
                {"fontBold", ui->tabWidget->font().bold()},
                {"fontItalic", ui->tabWidget->font().italic()}
            };
            QVariantMap fontSettings;
            fontSettings["fontSettings"] = innerFontSettings;
            emit multipleSettingsChanged(fontSettings);
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

        emit settingChanged("backgroundColor", m_currentColor.name());
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

// final save to ensure saving properly when closed
void InfoWindow::saveSettings() {
    QVariantMap innerFontSettings =
    {
        {"backgroundColor", m_currentColor.name()},

        {"fontFamily", ui->tabWidget->font().family()},
        {"fontSize", ui->tabWidget->font().pointSize()},
        {"fontColor",m_fontColor.name()},
        {"fontItalic", ui->tabWidget->font().italic()},
        {"fontBold", ui->tabWidget->font().bold()}
    };
    QVariantMap fontSettings;
    fontSettings["fontSettings"] = innerFontSettings;
    emit multipleSettingsChanged(fontSettings);
}

void InfoWindow::applySettings(const QJsonObject& settings) {
    qDebug() << "applying Settings";
    // Background color
    if (settings.contains("backgroundColor"))
        m_currentColor = QColor(settings["backgroundColor"].toString());

    // Font
    QFont font;
    if (settings.contains("fontSettings")) {
        QJsonObject fontSettings = settings["fontSettings"].toObject();

        if (fontSettings.contains("fontFamily")) {
            font.setFamily(fontSettings["fontFamily"].toString());
        }
        if (fontSettings.contains("fontSize")) {
            font.setPointSize(fontSettings["fontSize"].toInt());
        }
        if (fontSettings.contains("fontColor")) {
            m_fontColor = QColor(fontSettings["fontColor"].toString());
        }
        if (fontSettings.contains("fontItalic")) {
            font.setItalic(fontSettings["fontItalic"].toBool());
        }
        if (fontSettings.contains("fontBold")) {
            font.setBold(fontSettings["fontBold"].toBool());
        }
    }

    // Apply background
    QPalette palette;
    palette.setColor(QPalette::Window, m_currentColor);
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    // Apply font
    QString style = QString("color: %1; font-family: %2; font-size: %3pt; font-weight: %4; font-style: %5;")
        .arg(m_fontColor.name())
        .arg(font.family())
        .arg(font.pointSize())
        .arg(font.bold() ? "bold" : "normal")
        .arg(font.italic() ? "italic" : "normal");

    ui->tabWidget->setStyleSheet(style);
    ui->tabWidget->setFont(font);
}

//void InfoWindow::loadSettings() {
//    QFile file(path + "/settings.json");
//
//    if (file.open(QIODevice::ReadOnly)) {
//        QByteArray data = file.readAll();
//        file.close();
//
//        QJsonDocument doc = QJsonDocument::fromJson(data);
//        QJsonObject obj = doc.object();
//
//        if (obj.contains("backgroundColor"))
//            m_currentColor = QColor(obj["backgroundColor"].toString());
//        if (obj.contains("fontColor"))
//            m_fontColor = QColor(obj["fontColor"].toString());
//
//        QFont font;
//        font.setFamily(obj["fontFamily"].toString());
//        font.setPointSize(obj["fontSize"].toInt());
//        font.setBold(obj["fontBold"].toBool());
//        font.setItalic(obj["fontItalic"].toBool());
//
//        QPalette palette;
//        palette.setColor(QPalette::Window, m_currentColor);
//        this->setAutoFillBackground(true);
//        this->setPalette(palette);
//
//        QString style = QString("color: %1; font-family: %2; font-size: %3pt; font-weight: %4; font-style: %5;")
//            .arg(m_fontColor.name())
//            .arg(font.family())
//            .arg(font.pointSize())
//            .arg(font.bold() ? "bold" : "normal")
//            .arg(font.italic() ? "italic" : "normal");
//        ui->tabWidget->setStyleSheet(style);
//        ui->tabWidget->setFont(font);
//    }
//}






