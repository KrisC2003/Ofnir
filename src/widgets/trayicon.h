#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QIcon>

#include "infowindow.h"

class TrayIcon : public QSystemTrayIcon
{
	Q_OBJECT
public:
	explicit TrayIcon(QObject* parent);
	virtual ~TrayIcon() = default;

private:
	void initMenu();
	QMenu* m_menu;
	InfoWindow* m_infoWindow;
};