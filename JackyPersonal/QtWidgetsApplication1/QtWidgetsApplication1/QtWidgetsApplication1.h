#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QShortcut>
#include "ui_QtWidgetsApplication1.h"

#define HOTKEY_ID 1001

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

class QtWidgetsApplication1 : public QMainWindow
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
    QLabel* screenshotLabel;
    QTextEdit* textOutput;
    QShortcut* screenshotShortcut;
};
