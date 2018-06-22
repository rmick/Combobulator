#include "LttoMainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QSoundEffect>
#include "Defines.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setApplicationName( QString("The Combobulator") );
	QApplication::setDesktopSettingsAware(true);
	//QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QApplication    theApp(argc, argv);
    LttoMainWindow  lttoMainWindow;
	lttoMainWindow.setWindowIcon(QIcon(":/resources/images/Combobulator.ico"));
    qDebug() << "Starting the main application.";
	qDebug() << "------------------------------" << endl;
    QEventLoop      loop;
	QSoundEffect    sound_PowerUp;
    QPixmap         pixmap(":/resources/images/Combobulator Logo.jpg");
    QSplashScreen   splashScreen(pixmap);

#ifndef QT_DEBUG
	sound_PowerUp.setVolume(100);
	sound_PowerUp.setSource(QUrl::fromLocalFile(":/resources/audio/stinger-power-on.wav"));
	sound_PowerUp.play();
	splashScreen.show();
	QTimer::singleShot(2000, &loop, SLOT(quit()));
	loop.exec();
	lttoMainWindow.showFullScreen();
#else
	lttoMainWindow.show();
#endif

    splashScreen.finish(&lttoMainWindow);

    return theApp.exec();
}
