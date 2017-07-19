#include "LttoMainWindow.h"
#include <QApplication>
#include "TCPComms.h"
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName( QString("The Combobulator") );
    QApplication::setDesktopSettingsAware(false);
    QApplication::setStyle("plastique");
    QApplication theApp(argc, argv);
    LttoMainWindow  lttoMainWindow;
    lttoMainWindow.show();
    TCPComms tcpComms;
    return theApp.exec();
}
