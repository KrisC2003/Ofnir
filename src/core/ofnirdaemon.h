#pragma once
#include <QApplication>
#include <QObject>
#include "src/widgets/trayicon.h"
#include "src/widgets/screencapturewidget.h"

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
	QString m_historyFolderPath;
protected:
	bool changeHotkeys();
	bool saveToHistory();
	bool saveToClipboard();
private:
	void initTrayIcon();
	void initHotkeys();

	QScreen* m_screen;
	globalHotkeyFilter* hotkeyFilter = nullptr;
	TrayIcon* m_trayIcon = nullptr;
};