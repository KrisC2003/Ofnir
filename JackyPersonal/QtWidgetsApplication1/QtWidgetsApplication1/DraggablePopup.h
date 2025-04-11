#pragma once
#include <QWidget>
#include <QMouseEvent>
#include <QVBoxLayout>

class DraggablePopup : public QWidget
{
    Q_OBJECT

public:
    explicit DraggablePopup(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    QPoint dragPosition;
};
