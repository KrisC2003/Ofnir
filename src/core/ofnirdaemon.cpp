#include "src/settings/globalHotkeyFilter.h"
#include "src/widgets/infowindow.h"
#include "ofnirdaemon.h"

// Handle background related tasks and initialization
OfnirDaemon::OfnirDaemon(QObject* parent) 
    : QObject(parent) 
{
    initTrayIcon();
    QScreen* screen = QApplication::primaryScreen();
    m_captureWidget = new screenCaptureWidget(screen);
    m_captureWidget->hide();
    initHotkeys();
}

void OfnirDaemon::initTrayIcon() {
    m_trayIcon = new TrayIcon(this);
}

void OfnirDaemon::initHotkeys() {
    globalHotkeyFilter* nativeFilter = new globalHotkeyFilter(this);
    nativeFilter->registerShortcut();

    qApp->installNativeEventFilter(nativeFilter);
    connect(nativeFilter, &globalHotkeyFilter::hotkeyPressed, this, [this]() {
            //m_captureWidget->initScreenCapture();
            m_captureWidget->show();
        }
    );
}

bool OfnirDaemon::changeHotkeys() {
    return true;
}

bool OfnirDaemon::saveToClipboard() {
    return true;

}