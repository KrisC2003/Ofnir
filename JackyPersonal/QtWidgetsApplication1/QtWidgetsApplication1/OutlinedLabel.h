#pragma once

#include <QLabel>
#include <QPainter>

class OutlinedLabel : public QLabel
{
    Q_OBJECT

public:
    explicit OutlinedLabel(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
};
