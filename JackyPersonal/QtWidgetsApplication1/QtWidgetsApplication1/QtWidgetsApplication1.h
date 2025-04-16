// QtWidgetsApplication1.h
#pragma once
#include "qhotkey.h"
#include <QDialog>
#include <QFont>
#include <QColor>
#include <QKeySequence>
#include <QTextEdit>
#include <QShortcut>
#include <QPushButton>
#include "ui_QtWidgetsApplication1.h"

class QtWidgetsApplication1 : public QDialog

{
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget* parent = nullptr);
    ~QtWidgetsApplication1();

private slots:
    void captureAndShowScreenshot();

private:
    QWidget* createResultPopup(const QString& text, const QRect& anchorRect);

private:
    Ui::QtWidgetsApplication1Class ui;
    QTextEdit* textOutput;
    QHotkey* m_hotkey = nullptr;
};