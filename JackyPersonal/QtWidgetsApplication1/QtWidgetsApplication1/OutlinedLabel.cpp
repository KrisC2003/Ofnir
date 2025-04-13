#include "OutlinedLabel.h"

OutlinedLabel::OutlinedLabel(QWidget* parent)
    : QLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("font: bold 24px; color: white;");
}

void OutlinedLabel::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QFontMetrics fm(font());
    QRect textRect = rect();

    // outline
    QPen pen(Qt::black);
    pen.setWidth(20); 
    painter.setPen(pen);
    painter.drawText(textRect, alignment(), text());

    // inside
    painter.setPen(Qt::white);
    painter.drawText(textRect, alignment(), text());
}
