#pragma once
#include <QObject>
#include <QScreen>
#include <QApplication>
#include <QWidget>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QDateTime>
#include <QDir>

class QMouseEvent;
class QPaintEvent;
class QPixmap;

class screenCaptureWidget : public QWidget 
{
	Q_OBJECT
public:
	screenCaptureWidget(QScreen* screen, const QString& savePath, QWidget* parent = nullptr);
	~screenCaptureWidget() = default;
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	QRect updateRect();

	// Class Members
	QScreen* m_screen;

	QPixmap m_cachedPixmap;
	QString m_savePath;

	// Mouse tracking
	QPoint m_mousePressedPos;
	QPoint m_selectionStartPos;
	QPoint m_selectionEndPos;

	bool m_mouseIsPressed;
	bool m_overlayVisible;
signals:
	void screenshotCaptured(const QString& filePath);
};

