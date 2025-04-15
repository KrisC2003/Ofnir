#pragma once

#include <QWidget>


namespace Ui {
    class InfoWindow;
}

class InfoWindow : public QWidget
{
    Q_OBJECT

    public:
        explicit InfoWindow(QWidget* parent = nullptr);

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


    private:
        QColor m_currentColor;  // Add this line to track the selected background color
        QFont m_currentFont;
        QColor m_fontColor;

        Ui::InfoWindow* ui;

        QPoint m_dragPosition;
        bool m_dragging = false;  // Added this line
        int m_dragAreaHeight = 30;  // Define the draggable area height
};