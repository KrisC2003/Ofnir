#include "ScreenshotOverlay.h"
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent> 
ScreenshotOverlay::ScreenshotOverlay(QWidget* parent)
    : QWidget(parent)
{
    fullScreenPixmap = QGuiApplication::primaryScreen()->grabWindow(0);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setCursor(Qt::CrossCursor);
    resize(QGuiApplication::primaryScreen()->geometry().size());
    showFullScreen();
}

void ScreenshotOverlay::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, fullScreenPixmap);
    if (selecting) {
        QRect selectionRect(origin, end);
        painter.setBrush(QColor(0, 0, 0, 100));
        painter.setPen(QPen(Qt::red, 2));
        painter.drawRect(selectionRect.normalized());
    }
}

void ScreenshotOverlay::mousePressEvent(QMouseEvent* event)
{
    selecting = true;
    origin = end = event->pos();
    update();
}

void ScreenshotOverlay::mouseMoveEvent(QMouseEvent* event)
{
    if (selecting) {
        end = event->pos();
        update();
    }
}

void ScreenshotOverlay::mouseReleaseEvent(QMouseEvent* event)
{
    selecting = false;
    end = event->pos();
    QRect rect = QRect(origin, end).normalized();
    QPixmap selected = fullScreenPixmap.copy(rect);
    emit screenshotTakenWithRect(selected, rect.normalized());
    close();
}


void ScreenshotOverlay::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        close();  // press esc to close screenshot
    }
}