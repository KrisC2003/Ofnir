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
        const QString& fullText = block.first;
        QRect baseRect = block.second.translated(m_offsetRect.topLeft());
        qDebug() << fullText << " " << baseRect.topLeft();
        // Split by newline or sentence
        QStringList lines = fullText.split(QRegularExpression("[\\n\\r]+"), Qt::SkipEmptyParts);

        int verticalOffset = 0;

        int minFontSize = 8;
        int maxFontSize = 12;
        int scaledFontSize = qMin(m_offsetRect.width(), m_offsetRect.height()) / 4;

        for (const QString& line : lines) {
            QLabel* label = new QLabel(this);
            label->setText(line.trimmed());
            label->setStyleSheet("background-color: rgba(180,180,180,255); color: black; padding: 2px;");

            // Ensure font size is within reasonable bounds
            scaledFontSize = qBound(minFontSize, scaledFontSize, maxFontSize);

            label->setFont(QFont("Arial", scaledFontSize));
            label->setTextFormat(Qt::PlainText);  // respects \n literally
            label->setWordWrap(false);            // no wrapping
            label->adjustSize();

            QPoint labelPos = baseRect.topLeft() + QPoint(0, verticalOffset);
            if (label->width() > m_offsetRect.width()) {
                label->setFixedWidth(m_offsetRect.width());
            }

            if (label->height() > m_offsetRect.height()) {
                label->setFixedHeight(m_offsetRect.height());
            }

            label->move(labelPos);
            label->show();
            m_labels.push_back(label);
            verticalOffset += label->height() + lineSpacing;
        }
    }
    QPushButton* hideButton = new QPushButton("Hide Labels", this);
    int buttonWidth = 100;
    int buttonHeight = 30;

    // Position it at bottom-left of m_offsetRect
    int x = m_offsetRect.left();
    int y = m_offsetRect.bottom();

    hideButton->setGeometry(x, y, buttonWidth, buttonHeight);

    connect(hideButton, &QPushButton::clicked, this, &ResultOverlay::toggleLabelVisibility);
    hideButton->show();
}
void ResultOverlay::toggleLabelVisibility() {
    m_labelsVisible = !m_labelsVisible;
    for (QLabel* label : m_labels) {
        label->setVisible(m_labelsVisible);
    }
}
void ResultOverlay::keyPressEvent(QKeyEvent* event) {
    //qDebug() << event->key();
    if (event->key() == Qt::Key_Escape) {
        close();  // Triggers deletion
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