#include "LttoMainWindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QStandardPaths>
#include <QDateTime>
#include <QPixmap>
#include <QSoundEffect>
#include <QMessageBox>

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
        if(context.file == nullptr && context.function == nullptr) break;   // hides the annoying QAbstractSocket 'alreadyconnecting' message
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
            break;
        default:
            break;
        //abort();
    }

    QString logMessage = (QString)localMsg;

    //Send to Log file
    QString filePath = QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ).value(0);

    //qDebug() << "main::myMessageOutput() - FilePath =" << filePath;
    QDir thisDir;
    thisDir.setPath(filePath);
    QDir::setCurrent(filePath);
    QFile outFile("Combobulator_Log.txt");
    bool good = outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    thisDir.setPath(filePath);
    if (!thisDir.exists()) thisDir.mkpath(filePath);
    QDir::setCurrent(filePath);

    if(good)
    {
        QTextStream ts(&outFile);
        ts << logMessage << endl;
        outFile.close();
    }
    else qDebug() << "main::myMessageOutput() - Cannot create file" << thisDir << ":" << outFile;

    if(LttoMainWindow::textEditLogFile != nullptr)
    {
        //fprintf(stderr, "\t*** Writing to Log ***\n","","","","");
        LttoMainWindow::textEditLogFile->append(msg);
    }
}



int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setApplicationName( QString("Combobulator") );
    QApplication::setDesktopSettingsAware(true);
    qInstallMessageHandler(myMessageOutput);
    QApplication    theApp(argc, argv);

    qDebug() << "----------------------------------------------------------------------------" << endl;
    qDebug() << "----------------------------------------------------------------------------" << endl;
	qDebug() << "                        Starting the main application.";
	qDebug() << "                           " << BUILD_NUMBER << "-" << VERSION_NUMBER << endl;
	qDebug() << "\t" << QDateTime::currentDateTime();
    qDebug() << "----------------------------------------------------------------------------" << endl;
    qDebug() << "----------------------------------------------------------------------------" << endl;
    qDebug() << "----------------------------------------------------------------------------" << endl;
    qDebug() << "----------------------------------------------------------------------------" << endl;
    qDebug() << "----------------------------------------------------------------------------" << endl;
    qDebug() << "Creating main windoow";


#ifndef QT_DEBUG
    QEventLoop      loop;
    QSoundEffect    sound_PowerUp;
    QPixmap         pixmap(":/resources/images/Combobulator Logo.jpg");
    QSplashScreen   splashScreen(pixmap);

    theApp.processEvents();

    sound_PowerUp.setVolume(50);
    sound_PowerUp.setSource(QUrl::fromLocalFile(":/resources/audio/stinger-power-on.wav"));

    splashScreen.show();
    sound_PowerUp.play();
    QTimer::singleShot(2000, &loop, SLOT(quit()));
    loop.exec();
    splashScreen.close();
#endif

    LttoMainWindow  lttoMainWindow;
    lttoMainWindow.setWindowIcon(QIcon(":/resources/images/Combobulator.ico"));
    lttoMainWindow.showFullScreen();

    return theApp.exec();
}
