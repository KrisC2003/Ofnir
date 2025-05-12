#include "src/settings/globalHotkeyFilter.h"
#include "src/settings/settingsmanager.h"
#include "src/widgets/trayicon.h"
#include "src/widgets/screencapturewidget.h"
#include "src/widgets/resultoverlay.h"
#include "ocrmanager.h"
#include "ofnirdaemon.h"

// Handle background related tasks and initialization
OfnirDaemon::OfnirDaemon(QObject* parent)
    : QObject(parent)
    , m_screen(QApplication::primaryScreen())
    , m_folderPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
    , m_ocrManager(new OCRManager(this))
{
    m_settingsManager = new SettingsManager(m_folderPath, this);

    initTrayIcon();
    initHotkeys();
    initFolderPath();
}

void OfnirDaemon::initTrayIcon() {
    m_trayIcon = new TrayIcon(this, this);
}

void OfnirDaemon::initHotkeys() {
    m_hotkeyFilter = new GlobalHotkeyFilter(this);
    m_hotkeyFilter->registerShortcut();

    qApp->installNativeEventFilter(m_hotkeyFilter);
    connect(m_hotkeyFilter, &GlobalHotkeyFilter::hotkeyPressed, this, [this]() {
        screenCaptureWidget* widget = new screenCaptureWidget(m_screen, m_folderPath + "/ocr_history");
        connect(widget, &screenCaptureWidget::screenshotCaptured, this, &OfnirDaemon::handleScreenshotCaptured);
        }
    );
}

void OfnirDaemon::initFolderPath() {
    QDir dir;
    if (!dir.exists(m_folderPath)) {
        dir.mkpath(m_folderPath);  // Creates the folder if needed
    }
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

void OfnirDaemon::handleScreenshotCaptured(const QString imagePath, const QRect overlayOffset) {
    QVector<QPair<QString, QRect>> blockVector = m_ocrManager->processOCRWithConfidence(imagePath);
    if (blockVector.isEmpty()) {
        qWarning() << "Block data vector returned is empty";
    }
    else {
        ResultOverlay* widget = new ResultOverlay(blockVector, overlayOffset);
    }
}