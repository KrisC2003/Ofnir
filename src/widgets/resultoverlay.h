#pragma once

#include <QLabel>
#include <QPainter>
#include <QRegularExpression>
#include <QPushButton>
#include <QKeyEvent>

class ResultOverlay : public QWidget {
public:
	ResultOverlay(const QVector<QPair<QString, QRect>>& blockVector, const QRect& offsetRect, QWidget* parent = nullptr);
protected:
	void showQLabels();
	void createUtilButtons();
	void toggleLabelVisibility();
	void paintEvent(QPaintEvent* event) override;
private:
	QVector<QPair<QString, QRect>> m_blockVector;
	QVector<QLabel*> m_labels;

	QRect m_offsetRect;
	bool m_labelsVisible;
};