#include "screenCaptureWidget.hpp"
#include <QScreen>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>

screenCaptureWidget::screenCaptureWidget(QScreen* screen, QWidget* parent)
	: QWidget(parent)
	, m_screen(screen)
	, m_mouseIsPressed(false)
{
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//setWindowState(Qt::WindowFullScreen);
	setWindowState(Qt::WindowActive);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);
	setMouseTracking(true);
	activateWindow();
	showFullScreen();
}
//screenCaptureWidget::~screenCaptureWidget() {
//
//}
QRect screenCaptureWidget::updateRect() {
	return QRect(m_startMouseMovePos, m_endMouseMovePos).normalized();
};
void screenCaptureWidget::mousePressEvent(QMouseEvent* event) 
{
	m_mouseIsPressed = true;
	m_startMouseMovePos = event->pos();
	m_endMouseMovePos = event->pos();
	setCursor(Qt::CrossCursor);
}
void screenCaptureWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_mouseIsPressed) {
		return;
	}
	m_endMouseMovePos = event->pos();
	update();
}

void screenCaptureWidget::mouseReleaseEvent(QMouseEvent* event)
{
	m_mouseIsPressed = false;
	m_endMouseMovePos = event->pos();

	QRect selectedArea = updateRect();
	QPixmap pix_map = m_screen->grabWindow(0, selectedArea.x(),selectedArea.y(), selectedArea.width(), selectedArea.height());
	pix_map.save("screenshot.png", "PNG", 10);
	setCursor(Qt::ArrowCursor);
	close(); //closes after screenshot
}

void screenCaptureWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setPen(QPen(Qt::white, Qt::DashLine));
	painter.setBrush(QColor(0, 0, 0, 100));
	painter.drawRect(updateRect());
}

