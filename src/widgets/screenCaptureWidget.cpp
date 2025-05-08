#include "screenCaptureWidget.h"

screenCaptureWidget::screenCaptureWidget(QScreen* screen, const QString& savePath, QWidget* parent)
	: QWidget(parent)
	, m_screen(screen)
	, m_mouseIsPressed(false)
	, m_overlayVisible(true)
	, m_savePath(savePath)
{
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);
	setCursor(Qt::CrossCursor);
	setMouseTracking(true);
	showFullScreen();
	activateWindow();
}

QRect screenCaptureWidget::updateRect() {
	return QRect(m_selectionStartPos, m_selectionEndPos).normalized();
};

// TODO: try to reduce the flashing effect and also maybe not require repainting
void screenCaptureWidget::mousePressEvent(QMouseEvent* event) 
{	
	if (event->button() != Qt::LeftButton) return;

	m_selectionStartPos = m_selectionEndPos = event->pos();
	
	m_overlayVisible = false;
	repaint();
	m_cachedPixmap = m_screen->grabWindow(0);

	m_overlayVisible = true;
	update();
	m_mouseIsPressed = true;

	setCursor(Qt::CrossCursor);
}

void screenCaptureWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_mouseIsPressed) {
		return;
	}
	m_selectionEndPos = event->pos();
	update();
}

void screenCaptureWidget::mouseReleaseEvent(QMouseEvent* event)
{
	m_mouseIsPressed = false;
	QRect selectedArea = updateRect();
	m_selectionStartPos = QPoint();
	m_selectionEndPos = QPoint();

	QString filename = "screenshot_" + QDateTime::currentDateTime().toString("yyyy.MM.dd_HH-mm-ss") + ".png";

	QDir dir(m_savePath);
	QString fullPath = dir.filePath(filename);

	QPixmap capturedImg = m_cachedPixmap.copy(selectedArea);
	capturedImg.save(fullPath);

	setCursor(Qt::ArrowCursor);

	hide();
	emit screenshotCaptured(fullPath);
	deleteLater();

}

void screenCaptureWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	QRect selectedArea = updateRect();

	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.fillRect(rect(), Qt::transparent);

	if (m_overlayVisible) {
		painter.fillRect(rect(), QColor(0, 0, 0, 75));

		// clear rectangle applied to the dark overlay
		painter.setCompositionMode(QPainter::CompositionMode_Clear);
		painter.fillRect(selectedArea, Qt::transparent);

		// draws outline for rectangle
		painter.setCompositionMode(QPainter::CompositionMode_Source);
		QPen selectionPen(Qt::black, 2);
		selectionPen.setStyle(Qt::CustomDashLine);
		selectionPen.setDashPattern({ 3, 3 });
		painter.setPen(selectionPen);
		painter.drawRect(selectedArea);
	}

	if (m_mouseIsPressed) {
		painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
		painter.drawPixmap(0, 0, m_cachedPixmap);
	}
}

