#include "widgets/infowindow.h"
#include "settings/globalHotkeyfilter.h"
#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QObject>


// TODO: for the future in localization of app
void configureApp() {

}
// TODO: potentially separate tray icon step into its own widget
void setupSystemTray() {
    InfoWindow infoWindow;

    QIcon appIcon(":/icon.png");
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(appIcon);

    QMenu* menu = new QMenu();
    QAction* settingAction = new QAction("Setting", menu);
    QAction* quitAction = new QAction("Exit", menu);

    // connects Setting action
    QObject::connect(settingAction, &QAction::triggered, [&]() {
        infoWindow.show();
        infoWindow.raise();
        infoWindow.activateWindow();
        });
    // connects Exit action
    QObject::connect(quitAction, &QAction::triggered, &app, &QApplication::quit);
    // adds action to traymenu
    menu->addAction(settingAction);
    menu->addAction(quitAction);
    // makes tray icon in system tray
    trayIcon->setToolTip("Ofnir");
    trayIcon->setContextMenu(menu);
    trayIcon->show();
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);


    setupSystemTray();

    return app.exec();
}
