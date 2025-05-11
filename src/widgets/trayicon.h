#pragma once

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>

#include "infowindow.h"

class InfoWindow;
class SettingsManager;

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT
public:
	explicit TrayIcon(SettingsManager* settings, QObject* parent);
	virtual ~TrayIcon() = default;

private:
	void initMenu(SettingsManager* settings);
	QMenu* m_menu = nullptr;
	InfoWindow* m_infoWindow = nullptr;
};