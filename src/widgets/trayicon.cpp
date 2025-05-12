#include "infowindow.h"
#include "trayicon.h"


TrayIcon::TrayIcon(OfnirDaemon* daemon, QObject* parent) 
    : QSystemTrayIcon(parent)
{
    QIcon appIcon(":/icon.png");
    setIcon(appIcon);
    initMenu(daemon);
    setToolTip("Ofnir");
    setContextMenu(m_menu);
    show();
}

void TrayIcon::initMenu(OfnirDaemon* daemon) {
    m_menu = new QMenu();

    QAction* settingAction = new QAction("Setting", m_menu);
    connect(settingAction, &QAction::triggered, [this, daemon]() {
        m_infoWindow = new InfoWindow(daemon);
        m_infoWindow->show();
        m_infoWindow->raise();
        m_infoWindow->activateWindow();
        });

    QAction* quitAction = new QAction("Exit", m_menu);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);


    m_menu->addAction(settingAction);
    m_menu->addAction(quitAction);
}