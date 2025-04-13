#pragma once
#include <QMainWindow>
#include <QMouseEvent>
#include <QTabWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow();


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
    QColor currentColor;  // Add this line to track the selected background color
    QFont currentFont;
    QColor fontColor;

    Ui::MainWindow* ui;

    QPoint m_dragPosition;
    bool m_dragging = false;  // Added this line
    int m_dragAreaHeight = 30;  // Define the draggable area height
};