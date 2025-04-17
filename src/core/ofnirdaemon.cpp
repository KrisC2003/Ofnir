#include "src/settings/globalHotkeyFilter.h"
#include "src/widgets/infowindow.h"
#include "src/widgets/trayicon.h"
#include "ofnirdaemon.h"

// Handle background related tasks and initialization
OfnirDaemon::OfnirDaemon(QObject* parent) 
    : QObject(parent) 
{
    initTrayIcon();
}

void OfnirDaemon::initTrayIcon() {
    m_trayIcon = new TrayIcon(this);
    return;
}

void OfnirDaemon::initHotkeys() {

}

bool OfnirDaemon::changeHotkeys() {
    return true;
}

bool OfnirDaemon::saveToClipboard() {
    return true;

}