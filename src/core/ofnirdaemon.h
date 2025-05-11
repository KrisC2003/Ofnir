#pragma once
#include <QApplication>
#include <QObject>
#include <QDir>
#include <QThread>
#include <QStandardPaths>
#include "src/settings/globalHotkeyFilter.h"
#include "src/settings/settingsmanager.h"
#include "src/widgets/infowindow.h"
#include "src/widgets/trayicon.h"
#include "src/widgets/screencapturewidget.h"
#include "src/widgets/resultoverlay.h"
#include "ocrmanager.h"

class globalHotkeyFilter;
class screenCaptureWidget;
class TrayIcon;

// should handle initialization and background processes (like data)

class OfnirDaemon : public QObject 
{
	Q_OBJECT
public:
	explicit OfnirDaemon(QObject* parent = nullptr);
	virtual ~OfnirDaemon() = default;
	QString m_folderPath;
protected:
	bool changeHotkeys();
	bool saveToHistory();
	bool saveToClipboard();
private:
	void initTrayIcon();
	void initHotkeys();
	void initFolderPath();

	OCRManager* m_ocrManager;
	SettingsManager* m_settingsManager;

	QScreen* m_screen;
	globalHotkeyFilter* hotkeyFilter = nullptr;
	TrayIcon* m_trayIcon = nullptr;

private slots:
	void handleScreenshotCaptured(const QString imagePath, const QRect overlayOffset);
};