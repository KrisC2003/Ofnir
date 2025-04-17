#include "widgets/infowindow.h"
#include "core/ofnirdaemon.h"
#include "settings/globalHotkeyfilter.h"
#include <QApplication>



// TODO: for the future in localization of app
void configureApp() {

}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Ofnir");

    auto daemon = new OfnirDaemon(&app); // starts daemon and initializes the app, parented to app to ensure deletion on app close

    return app.exec();
}
