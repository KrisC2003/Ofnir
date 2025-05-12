#pragma once

#include <QDialog>
#include <QColor>
#include <QPoint>
#include <QListWidget> 
#include <QIcon>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QFileInfo>
#include <QStringList>
#include <QColorDialog>
#include <QPalette>
#include <QFontDialog>
#include <QJsonObject>
#include <QFile>
#include <QStandardPaths>
#include <QMessageBox>
#include <QJsonArray>

class OfnirDaemon;
class FileListWidget;

namespace Ui {
    class InfoWindow;
}

class InfoWindow : public QDialog
{
    Q_OBJECT

    public:
        explicit InfoWindow(OfnirDaemon* daemon, QDialog* parent = nullptr);
        ~InfoWindow();

    protected:
        void closeEvent(QCloseEvent* event) override;
        void mousePressEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;

    private slots:
        void changeBackgroundColor();
        void changeFont();
        void onTabChanged(int index);
        void importFile();

    private:
        void saveSettings();       
        //void loadSettings();
        void loadImportedFiles();
        void saveImportedFiles();

        OfnirDaemon* ofnirDaemon = nullptr;

        QColor m_currentColor;  
        QFont m_currentFont;
        QColor m_fontColor;
        QStringList m_importedFiles;

        FileListWidget* customList = nullptr;

        Ui::InfoWindow* ui;

        QPoint m_dragPosition;
        bool m_dragging = false;  
        int m_dragAreaHeight = 30;  // Define the draggable area height

    private slots: 
        void applySettings(const QJsonObject& settings);

    signals:
        void settingChanged(const QString& key, const QVariant& value);
        void multipleSettingsChanged(const QVariantMap& settingsMap);

};

