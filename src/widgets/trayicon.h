#pragma once

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>

class InfoWindow;
class OfnirDaemon;

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT
public:
	explicit TrayIcon(OfnirDaemon* daemon, QObject* parent);
	virtual ~TrayIcon() = default;

private:
	void initMenu(OfnirDaemon* daemon);
	QMenu* m_menu = nullptr;
	InfoWindow* m_infoWindow = nullptr;
};