#ifndef LTTOCOMMS_H
#define LTTOCOMMS_H

#include <QObject>
#include <QTimer>
#include "Defines.h"
#include "LazerSwarm.h"

#include "SerialUSBcomms.h"

class LttoComms : public QObject
{
    Q_OBJECT

public:
    explicit    LttoComms(QObject *parent = 0);

    bool        sendPacket(char type, int data = 0, bool dataFormat = false);

    bool        getUseLazerSwarm() const;
    void        setUseLazerSwarm(bool value);

    bool getDontAnnounceGame() const;
    void setDontAnnounceGame(bool value);

    int getMissedAnnounceCount() const;
    void setMissedAnnounceCount(int value);

signals:
    void        RequestJoinGame(int Game, int Tagger, int Flags);
    void        AckPlayerAssignment(int Game, int Tagger);
    void        TimerBlock(bool StartStop);
    void        sendSerialData(QByteArray dataToSend);

private slots:
    void        receivePacket(QByteArray RxData);

private:

    int             calculatedCheckSumTx;
    int             calculatedCheckSumRx;
    QByteArray      irDataIn;
    QByteArrayList  rxPacketList;
    QString         packetString;
    bool            useLazerSwarm;
    bool            dontAnnounceGame;
    int             missedAnnounceCount;

    void            processPacket(QList<QByteArray> data);
    int             extract(QList<QByteArray> &data);
    void            findSerialPort();
    bool            isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum);
    int             ConvertDecToBCD(int dec);
    int             ConvertBCDtoDec(int bcd);
    QString         createIRstring(int data);
    void            blockingDelay(int mSec);
};

extern LttoComms lttoComms;

#endif // LTTOCOMMS_H
