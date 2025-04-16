#pragma once

#include <QLabel>
#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>

//OutlinedLabel:
class OutlinedLabel : public QLabel
{
    Q_OBJECT

public:
    explicit OutlinedLabel(QWidget* parent = nullptr)
        : QLabel(parent),
        m_textColor(Qt::white),
        m_outlineColor(Qt::black),
        m_outlineWidth(2)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

    void setOutlineColor(const QColor& color) { m_outlineColor = color; update(); }
    void setTextColor(const QColor& color) { m_textColor = color; update(); }
    void setOutlineWidth(int width) { m_outlineWidth = width; update(); }

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        QRect textRect = rect();
        QString content = text();

        // Draw outline
        painter.setPen(QPen(m_outlineColor));
        for (int dx = -m_outlineWidth; dx <= m_outlineWidth; ++dx) {
            for (int dy = -m_outlineWidth; dy <= m_outlineWidth; ++dy) {
                if (dx == 0 && dy == 0) continue;
                painter.drawText(textRect.translated(dx, dy), int(alignment()), content);
            }
        }

        // Draw main text
        painter.setPen(QPen(m_textColor));
        painter.drawText(textRect, int(alignment()), content);
    }

private:
    QColor m_outlineColor;
    QColor m_textColor;
    int m_outlineWidth;
};

//DraggablePopup
class DraggablePopup : public QWidget
{
    Q_OBJECT

public:
    explicit DraggablePopup(QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
    void mousePressEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton)
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        if (event->buttons() & Qt::LeftButton)
            move(event->globalPosition().toPoint() - m_dragPosition);
    }

private:
    QPoint m_dragPosition;
};
