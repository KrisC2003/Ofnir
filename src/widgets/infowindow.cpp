#include "infowindow.h"
#include "ui_infowindow.h"
#include "src/settings/globalHotkeyFilter.h"
#include "screenCaptureWidget.h"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QColorDialog>
#include <QPalette>
#include <QFontDialog>

#define HOTKEY_ID 1001

InfoWindow::InfoWindow(QDialog* parent)
    : QDialog(parent), ui(new Ui::InfoWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    //connects ui buttons to functions
    connect(ui->closeButton, &QPushButton::clicked, this, &InfoWindow::close);
    connect(ui->colorButton, &QPushButton::clicked, this, &InfoWindow::changeBackgroundColor);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &InfoWindow::onTabChanged);
    connect(ui->fontStyle, &QPushButton::clicked, this, &InfoWindow::changeFont);
}

InfoWindow::~InfoWindow() {
    delete ui;
}

void InfoWindow::onTabChanged(int index)
{
    qDebug() << "Tab changed to Theme: " << index;
}

//CHANGE FONT BUTTON
void InfoWindow::changeFont() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);

    if (ok) {
        QColor color = QColorDialog::getColor(m_fontColor, this, "Select Font Color");

        if (color.isValid()) {
            m_fontColor = color;
            QString style = QString("color: %1; font-family: %2; font-size: %3pt; font-weight: %4; font-style: %5;")
                .arg(m_fontColor.name())   // Apply font color
                .arg(font.family())      // Apply font fmaily
                .arg(font.pointSize())   // Apply size
                .arg(font.weight() == QFont::Bold ? "bold" : "normal") // Apply bold
                .arg(font.italic() ? "italic" : "normal"); // Apply italic
            ui->tabWidget->setStyleSheet(style);
            ui->tabWidget->setFont(font);
        }
    }
}

// CHANGE BACKGROUND COLOR BUTTON
void InfoWindow::changeBackgroundColor() {
    QColor color = QColorDialog::getColor(m_currentColor, this, "Select Background Color");

    if (color.isValid()) {
        m_currentColor = color;
        QPalette palette;
        palette.setColor(QPalette::Window, m_currentColor);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
    }
}

// Close event override as hideg
void InfoWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    this->hide();
}

// Mouse press events (For dragging)
void InfoWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && event->pos().y() <= m_dragAreaHeight) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        m_dragging = true;  // Enable dragging
        event->accept();
    }
    else {
        m_dragging = false;  // Disable dragging if clicked outside the draggable area
        event->ignore();
    }
}

void InfoWindow::mouseMoveEvent(QMouseEvent* event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
    else {
        event->ignore();
    }
}
void InfoWindow::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;  // Stop dragging when mouse is released
        event->accept();
    }
}


