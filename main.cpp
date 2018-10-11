#include "LttoMainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QSoundEffect>
#include "Defines.h"

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	// Replicate the messages to DeBug for local use
	QByteArray localMsg = msg.toLocal8Bit();
	switch (type)
	{
		case QtDebugMsg:
			//fprintf(stderr, "Debug:\t%s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			fprintf(stderr, "Debug:\t%s\n", localMsg.constData());
			break;
		case QtWarningMsg:
		if(context.file == nullptr && context.function == nullptr) // hides the annoying QAbstractSocket 'alreadyconnecting' message
			break;
			//fprintf(stderr, "Warning:\t%s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			fprintf(stderr, "Warning:\t%s\n", localMsg.constData());
		break;
		case QtCriticalMsg:
			//fprintf(stderr, "Critical:\t%s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			fprintf(stderr, "Critical:\t%s\n", localMsg.constData());
		break;
		case QtInfoMsg:
			//fprintf(stderr, "Info:\t%s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			fprintf(stderr, "Info:\t%s\n", localMsg.constData());
		break;
		case QtFatalMsg:
			//fprintf(stderr, "Fatal:\t%s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
			fprintf(stderr, "Fatal:\t%s\n", localMsg.constData());
		abort();
	}

	QString logMessage = (QString)localMsg;

	//Send to Log file
	QString filePath = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).value(0);
	//qDebug() << "FilePath =" << filePath;
	QDir thisDir;
	//thisDir.setPath(filePath);
thisDir.setPath("user/Richie/Desktop");
	//if (!thisDir.exists()) thisDir.mkpath(filePath);
	QDir::setCurrent("users/Richie/Desktop");
	QFile outFile("Combobulator_Log.txt");
	bool good = outFile.open(QIODevice::WriteOnly | QIODevice::Append);
	if(good)
	{
		QTextStream ts(&outFile);
		ts << logMessage << endl;
		outFile.close();
	}



	if(LttoMainWindow::textEditLogFile != nullptr)
	{
		//fprintf(stderr, "\t*** Writing to Log ***\n","","","","");
		LttoMainWindow::textEditLogFile->append(msg);
	}
}




int main(int argc, char *argv[])
{
	QCoreApplication::setApplicationName( QString("Combobulator") );
	QApplication::setDesktopSettingsAware(true);
	//qInstallMessageHandler(myMessageOutput);
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
