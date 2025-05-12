#include "resultoverlay.h"

ResultOverlay::ResultOverlay(const QVector<QPair<QString, QRect>>& blockVector, const QRect& offsetRect, QWidget* parent)
    : QWidget(parent)

    , m_blockVector(blockVector)
    , m_offsetRect(offsetRect)
    , m_labelsVisible(true)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setFocusPolicy(Qt::StrongFocus);
    showFullScreen();
    raise();
    activateWindow();
    setFocus();

    showQLabels();

	update();
}


void ResultOverlay::showQLabels() {
    int lineSpacing = 2;
    for (const QPair<QString, QRect>& block : m_blockVector) {
        QString fullText = block.first;
        QRect baseRect = block.second.translated(m_offsetRect.topLeft());
        qDebug() << fullText << " " << baseRect.topLeft();

        int minFontSize = 8;
        int maxFontSize = 12;
        int scaledFontSize = qMin(m_offsetRect.width(), m_offsetRect.height()) / 4;
        scaledFontSize = qBound(minFontSize, scaledFontSize, maxFontSize);

        // Step 1: Group texts by QPoint
        QHash<QPoint, QStringList> textMap;

        for (const QPair<QString, QRect>& block : m_blockVector) {
            QPoint pos = block.second.translated(m_offsetRect.topLeft()).topLeft();
            QString cleanText = block.first.trimmed();
            if (!cleanText.isEmpty()) {
                textMap[pos].append(cleanText);
            }
        }
        for (auto it = textMap.constBegin(); it != textMap.constEnd(); ++it) {
            const QPoint& pos = it.key();
            const QStringList& lines = it.value();
            QString mergedText = lines.join("\n");

            QLabel* label = new QLabel(this);
            label->setText(mergedText);
            label->setStyleSheet("background-color: rgba(180,180,180,255); color: black; padding: 2px;");
            label->setFont(QFont("Arial", scaledFontSize));
            label->setTextFormat(Qt::PlainText); // Keep \n as literal
            label->setWordWrap(false);
            label->adjustSize();

            if (label->width() > m_offsetRect.width())
                label->setFixedWidth(m_offsetRect.width());

            if (label->height() > m_offsetRect.height())
                label->setFixedHeight(m_offsetRect.height());

            label->move(pos);
            label->show();
            m_labels.push_back(label);
        }
    }
    createUtilButtons();
}

void ResultOverlay::createUtilButtons() {
    QPushButton* hideButton = new QPushButton("Hide Labels", this);
    QPushButton* closeButton = new QPushButton("Close", this);

    // Position it at bottom-left of m_offsetRect
    int x = m_offsetRect.left();
    int y = m_offsetRect.bottom();

    int buttonSpacing = 10;
    closeButton->setGeometry(x, y, 70, 30);
    hideButton->setGeometry(x + closeButton->width() + buttonSpacing, y, 100, 30);
    connect(closeButton, &QPushButton::clicked, this, [this]() { close(); });
    connect(hideButton, &QPushButton::clicked, this, &ResultOverlay::toggleLabelVisibility);
    closeButton->show();
    hideButton->show();
}

void ResultOverlay::toggleLabelVisibility() {
    m_labelsVisible = !m_labelsVisible;
    for (QLabel* label : m_labels) {
        label->setVisible(m_labelsVisible);
    }
}

void ResultOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::white, 2));

    QFont font = painter.font();
    font.setPointSize(10);
    painter.setFont(font);
    painter.drawRect(m_offsetRect);
}