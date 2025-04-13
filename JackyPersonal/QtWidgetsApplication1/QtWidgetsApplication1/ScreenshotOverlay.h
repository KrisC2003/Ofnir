#pragma once
#include <QWidget>
#include <QPixmap>
#include <QPoint>

class ScreenshotOverlay : public QWidget
{
    Q_OBJECT
public:
    ScreenshotOverlay(QWidget* parent = nullptr);
signals:
    void screenshotTaken(const QPixmap& pixmap);
signals:
    void screenshotTakenWithRect(const QPixmap& pixmap, const QRect& rect);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;  
private:
    QPixmap fullScreenPixmap;
    QPoint origin, end;
    bool selecting = false;
};
