#include "widgets/mainwindow.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QObject>



int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QIcon appIcon("://icon.png");
    app.setWindowIcon(appIcon);


    MainWindow w;
    MainWindow mainWindow;
    //System Tray things
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(QIcon("://icon.png"));
    trayIcon->setToolTip("Ofnir");

    QMenu* menu = new QMenu();
    QAction* settingAction = new QAction("Setting", menu);
    QAction* quitAction = new QAction("Exit", menu);

    //Buttons on system tray
    QObject::connect(settingAction, &QAction::triggered, [&]() {
        mainWindow.show();
        mainWindow.raise();
        mainWindow.activateWindow();
        });

    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);

    menu->addAction(settingAction);
    menu->addAction(quitAction);

    trayIcon->setContextMenu(menu);
    trayIcon->show();

    return app.exec();
}
