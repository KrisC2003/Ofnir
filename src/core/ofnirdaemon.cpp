#include "src/settings/globalHotkeyFilter.h"
#include "src/widgets/infowindow.h"
#include "ofnirdaemon.h"
#include <QDir>
#include <QStandardPaths>

// Handle background related tasks and initialization
OfnirDaemon::OfnirDaemon(QObject* parent) 
    : QObject(parent) 
{
    initTrayIcon();
    initHotkeys();
    m_screen = QApplication::primaryScreen();
    m_historyFolderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/ocr_history";
    QDir dir;
    if (!dir.exists(m_historyFolderPath)) {
        dir.mkpath(m_historyFolderPath);  // Creates the folder if needed
    }
}

void OfnirDaemon::initTrayIcon() {
    m_trayIcon = new TrayIcon(this);
}

void OfnirDaemon::initHotkeys() {
    globalHotkeyFilter* nativeFilter = new globalHotkeyFilter(this);
    nativeFilter->registerShortcut();

    qApp->installNativeEventFilter(nativeFilter);
    connect(nativeFilter, &globalHotkeyFilter::hotkeyPressed, this, [this]() {
            screenCaptureWidget* widget = new screenCaptureWidget(m_screen, m_historyFolderPath);
        }
    );
}

bool OfnirDaemon::changeHotkeys() {
    return true;
}

    
bool OfnirDaemon::saveToClipboard() {
    return true;
}

bool OfnirDaemon::saveToHistory() {

    return true;
}