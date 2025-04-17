#pragma once
#include <QApplication>
#include <QObject>

class TrayIcon;
class ScreenCaptureWidget;

// should handle initialization and background processes (like data)

class OfnirDaemon : public QObject 
{
	Q_OBJECT
public:
	explicit OfnirDaemon(QObject* parent = nullptr);
	virtual ~OfnirDaemon() = default;
protected:
	bool changeHotkeys();
	bool saveToClipboard();
private:
	void initTrayIcon();
	void initHotkeys();

	TrayIcon* m_trayIcon;
};