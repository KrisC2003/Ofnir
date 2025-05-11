#include "trayicon.h"


TrayIcon::TrayIcon(SettingsManager* settings, QObject* parent) 
    : QSystemTrayIcon(parent)
{
    QIcon appIcon(":/icon.png");
    setIcon(appIcon);
    initMenu(settings);
    setToolTip("Ofnir");
    setContextMenu(m_menu);
    show();
}

void TrayIcon::initMenu(SettingsManager* settings) {
    m_menu = new QMenu();

    QAction* settingAction = new QAction("Setting", m_menu);
    connect(settingAction, &QAction::triggered, [this, settings]() {
        m_infoWindow = new InfoWindow(settings);
        m_infoWindow->show();
        m_infoWindow->raise();
        m_infoWindow->activateWindow();
        });

    QAction* quitAction = new QAction("Exit", m_menu);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);


    m_menu->addAction(settingAction);
    m_menu->addAction(quitAction);
}