#include "screenCaptureWidget.h"

screenCaptureWidget::screenCaptureWidget(QScreen* screen, QWidget* parent)
	: QWidget(parent)
	, m_screen(screen)
	, m_mouseIsPressed(false)
{
	setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Tool | Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_DeleteOnClose);
	setCursor(Qt::CrossCursor);
	setMouseTracking(true);
	activateWindow();
	showFullScreen();

	if (m_screen) {
		m_overlay = QPixmap(size());

		m_darkOverlay = QPixmap(size());
		m_darkOverlay.fill(QColor(0, 0, 0, 75));
	}
}
//screenCaptureWidget::~screenCaptureWidget() {
//
//}

void screenCaptureWidget::initScreenCapture() {

}

QRect screenCaptureWidget::updateRect() {
	return QRect(m_selectionStartPos, m_selectionEndPos).normalized();
};

void screenCaptureWidget::mousePressEvent(QMouseEvent* event) 
{
	if (event->button() != Qt::LeftButton) return;

	m_mouseIsPressed = true;
	m_selectionStartPos = m_selectionEndPos = event->pos();
	m_cachedPixmap = m_screen->grabWindow(0);
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
	m_selectionEndPos = event->pos();
	// ensures hide is processed before screenshot
	hide(); 
	QApplication::processEvents();

	QRect selectedArea = updateRect();
	QPixmap pix_map = m_screen->grabWindow(0, selectedArea.x(),selectedArea.y(), selectedArea.width(), selectedArea.height());
	pix_map.save("screenshot.png", "PNG", 10);

	setCursor(Qt::ArrowCursor);
	close(); //closes after screenshot
}

void screenCaptureWidget::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	QRect selectedArea = updateRect();

	// source is there to ignore the translucent background blend
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.drawPixmap(0,0,m_cachedPixmap);

	m_overlay.fill(Qt::transparent);

	// draws dark overlay (source mode to ignore previous alpha channels (resets)
	QPainter overlayPainter(&m_overlay);
	overlayPainter.setCompositionMode(QPainter::CompositionMode_Source);
	overlayPainter.drawPixmap(0,0,m_darkOverlay);
	// clear rectangle applied to the dark overlay
	overlayPainter.setCompositionMode(QPainter::CompositionMode_Clear);
	overlayPainter.fillRect(selectedArea, Qt::transparent); 
	overlayPainter.end();

	// draws dark overlay OVER the pixmap
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
	painter.drawPixmap(0, 0, m_overlay);
	// draws outline for rectangle
	QPen selectionPen(Qt::black, 2);
	selectionPen.setStyle(Qt::CustomDashLine);
	selectionPen.setDashPattern({ 3, 3 });
	painter.setPen(selectionPen);
	painter.drawRect(selectedArea);
}

