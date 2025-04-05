#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QColorDialog>
#include <QPalette>
#include <QFontDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);


    setWindowFlags(Qt::FramelessWindowHint);
    connect(ui->closeButton, &QPushButton::clicked, this, &QMainWindow::close);
    connect(ui->colorButton, &QPushButton::clicked, this, &MainWindow::changeBackgroundColor);  // Connect button to slot
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
    connect(ui->fontStyle, &QPushButton::clicked, this, &MainWindow::changeFont);





}

void MainWindow::onTabChanged(int index)
{
    qDebug() << "Tab changed to Theme: " << index;
}

//CHANGE FONT BUTTON
void MainWindow::changeFont() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);

    if (ok) {
        QColor color = QColorDialog::getColor(fontColor, this, "Select Font Color");

        if (color.isValid()) {       
            fontColor = color;
            QString style = QString("color: %1; font-family: %2; font-size: %3pt; font-weight: %4; font-style: %5;")
                                .arg(fontColor.name())   // Apply font color
                                .arg(font.family())      // Apply font fmaily
                                .arg(font.pointSize())   // Apply size
                                .arg(font.weight() == QFont::Bold ? "bold" : "normal") // Apply bold
                                .arg(font.italic() ? "italic" : "normal"); // Apply italic
            ui->centralwidget->setStyleSheet(style);
            ui->centralwidget->setFont(font);
        }
    }
}



// CHANGE BACKGROUND COLOR BUTTON
void MainWindow::changeBackgroundColor() {
    QColor color = QColorDialog::getColor(currentColor, this, "Select Background Color");

    if (color.isValid()) {
        currentColor = color;
        QPalette palette;
        palette.setColor(QPalette::Window, currentColor);
        this->setAutoFillBackground(true);
        this->setPalette(palette);
    }
}


// Close event override as hide
void MainWindow::closeEvent(QCloseEvent *event) {
    event->ignore();
    this->hide();
}

// Mouse press (For dragging)
void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && event->pos().y() <= m_dragAreaHeight) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        m_dragging = true;  // Enable dragging
        event->accept();
    } else {
        m_dragging = false;  // Disable dragging if clicked outside the draggable area
        event->ignore();
    }
}

// Move move (For dragging)
void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    } else {
        event->ignore();
    }
}

// Move release (For dragging)
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;  // Stop dragging when mouse is released
        event->accept();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}
