#include "LttoMainWindow.h"
#include <QApplication>
//#include "Game.h"
//#include "Players.h"
#include "TCPComms.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LttoMainWindow  w;
    w.show();
    TCPComms tcpComms;
    return a.exec();
}
