#pragma once
#include "qhotkey.h"
#include <QDialog>
#include <QTableWidget>
#include <QPushButton>

class QtWidgetsApplication1 : public QDialog {
    Q_OBJECT

public:
    QtWidgetsApplication1(QWidget* parent = nullptr);
    ~QtWidgetsApplication1();

private slots:
    void captureAndShowScreenshot();
    void showHistoryDialog(); 

private:
    QWidget* createResultPopup(const QString& text, const QRect& anchorRect);

    QHotkey* m_hotkey = nullptr;
    QDialog* m_historyDialog = nullptr;      
    QTableWidget* m_historyTable = nullptr;  
};
