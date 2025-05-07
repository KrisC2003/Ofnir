#pragma once

#include <QDialog>
#include <QColor>
#include <QPoint>
#include <QListWidget> 
#include "filelistwidget.h"

namespace Ui {
    class InfoWindow;
}

class InfoWindow : public QDialog
{
    Q_OBJECT

    public:
        explicit InfoWindow(QDialog* parent = nullptr);

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
        void loadSettings();
        void loadImportedFiles();
        void saveImportedFiles();
        QColor m_currentColor;  
        QFont m_currentFont;
        QColor m_fontColor;
        QStringList m_importedFiles;

        FileListWidget* customList = nullptr;
 


        Ui::InfoWindow* ui;

        QPoint m_dragPosition;
        bool m_dragging = false;  
        int m_dragAreaHeight = 30;  // Define the draggable area height
    
};

