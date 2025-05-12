#pragma once
#include <QApplication>
#include <QObject>
#include <QDir>
#include <QThread>
#include <QStandardPaths>

class GlobalHotkeyFilter;
class SettingsManager;
class TrayIcon;
class ScreenCaptureWidget;
class OCRManager;
class ResultOverlay;
// should handle initialization and background processes (like data)

class OfnirDaemon : public QObject 
{
	Q_OBJECT
public:
	explicit OfnirDaemon(QObject* parent = nullptr);
	virtual ~OfnirDaemon() = default;

	SettingsManager* settingsManager() const { return m_settingsManager; }
	GlobalHotkeyFilter* hotkeyFilter() const { return m_hotkeyFilter;  }

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

	QString m_folderPath;

	QScreen* m_screen;
	GlobalHotkeyFilter* m_hotkeyFilter = nullptr;
	TrayIcon* m_trayIcon = nullptr;

private slots:
	void handleScreenshotCaptured(const QString imagePath, const QRect overlayOffset);
};