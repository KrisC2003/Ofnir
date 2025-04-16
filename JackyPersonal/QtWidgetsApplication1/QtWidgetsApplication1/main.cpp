#include <QApplication>
#include "QtWidgetsApplication1.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QtWidgetsApplication1 mainWindow;
    mainWindow.show();  

    return app.exec();
}
