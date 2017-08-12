#include "LttoMainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QThread>
#include "TCPComms.h"
#include <QFile>

class mofta : public QThread
{
public:
    static void sleep(unsigned long secs) { QThread::sleep(secs); }
};

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName( QString("The Combobulator") );
    QApplication::setDesktopSettingsAware(false);

    QApplication    theApp(argc, argv);
    LttoMainWindow  lttoMainWindow;
    QEventLoop      loop;
    QSoundEffect    sound_PowerUp;
    QPixmap         pixmap(":/resources/images/Combobulator Logo.jpg");
    QSplashScreen   splash(pixmap);
    TCPComms        tcpComms;

    sound_PowerUp.setSource(QUrl::fromLocalFile(":/resources/audio/stinger-power-on.wav"));
    sound_PowerUp.setVolume(1.0);

    sound_PowerUp.play();
    splash.show();

    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();

    lttoMainWindow.show();
    splash.finish(&lttoMainWindow);

    return theApp.exec();
}
