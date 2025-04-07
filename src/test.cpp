#include <QApplication>
#include <QSystemTrayIcon>
#include "screenCaptureWidget.hpp"
#include <QPushButton>
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QScreen* screen = QApplication::primaryScreen();

    screenCaptureWidget captureWidget(screen);
    captureWidget.show();  // Ensure the widget is visible to the user

    return app.exec();
}