#pragma once

#include <QLabel>
#include <QPainter>
#include <QRegularExpression>
struct BlockData;

class ResultOverlay : public QWidget {
public:
	ResultOverlay(const QVector<QPair<QString, QRect>>& blockVector, const QRect& offsetRect, QWidget* parent = nullptr);
protected:
	void showQLabels();
	void paintEvent(QPaintEvent* event) override;
private:
	QVector<QPair<QString, QRect>> m_blockVector;
	QRect m_offsetRect;
};