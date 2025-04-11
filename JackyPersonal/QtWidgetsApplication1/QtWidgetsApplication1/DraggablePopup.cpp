#include "DraggablePopup.h"

DraggablePopup::DraggablePopup(QWidget* parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
}

void DraggablePopup::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
}

void DraggablePopup::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
        move(event->globalPosition().toPoint() - dragPosition);
}
