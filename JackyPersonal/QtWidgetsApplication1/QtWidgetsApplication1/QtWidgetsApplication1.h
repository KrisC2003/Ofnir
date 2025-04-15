#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QShortcut>
#include <QKeySequence>
#include "ui_QtWidgetsApplication1.h"

#define HOTKEY_ID 1001

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

class QtWidgetsApplication1 : public QMainWindow
{
    Q_OBJECT

public:
    explicit QtWidgetsApplication1(QWidget* parent = nullptr);
    ~QtWidgetsApplication1();

private slots:
    void captureAndShowScreenshot();

private:
    QWidget* createResultPopup(const QString& text, const QRect& anchorRect);
    void applyUserHotkey();  

private:
    Ui::QtWidgetsApplication1Class ui;
    QShortcut* screenshotShortcut = nullptr;
};
