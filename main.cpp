#include "lttomainwindow.h"
#include <QApplication>
#include "Game.h"
#include "players.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LttoMainWindow w;
    w.show();

    return a.exec();
}
