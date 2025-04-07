#pragma once
#include <QObject>
#include <QScreen>
#include <QApplication>
#include <QWidget>

class QMouseEvent;
class QPaintEvent;
class QPixmap;

class screenCaptureWidget : public QWidget 
{
	Q_OBJECT
public:
	screenCaptureWidget(QScreen* screen, QWidget* parent = nullptr);

	~screenCaptureWidget() = default;

private slots:
public:

protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;

private:
	QRect updateRect();


	// Class Members
	QScreen* m_screen;


	// Mouse tracking
	QPoint m_mousePressedPos;
	QPoint m_startMouseMovePos;
	QPoint m_endMouseMovePos;

	bool m_mouseIsPressed;

};

