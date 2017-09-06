#ifndef LTTOCOMMS_H
#define LTTOCOMMS_H

#include <QObject>
#include <QTimer>
#include "Defines.h"
#include "LazerSwarm.h"

#include "SerialUSBcomms.h"
#include "TCPComms.h"

class LttoComms : public QObject
{
    Q_OBJECT

public:
    explicit    LttoComms(QObject *parent = 0);

    bool        sendPacket(char type, int data = 0, bool dataFormat = false);
    void sendLCDtext(QString textToSend, int lineNumber);

    bool        getUseLazerSwarm() const;
    void        setUseLazerSwarm(bool value);

    bool        getDontAnnounceGame() const;
    void        setDontAnnounceGame(bool value);

    int         getMissedAnnounceCount() const;
    void        setMissedAnnounceCount(int value);

    void        androidRxPacket(QByteArray data);

    bool        getTcpCommsConnected() const;
    void        setTcpCommsConnected(bool value);

    bool        getDontAnnounceFailedSignal() const;
    void        setDontAnnounceFailedSignal(bool value);

    void        nonBlockingDelay(int mSec);

    bool        getIsUSBinitialised() const;
    void        setIsUSBinitialised(bool value);

    int         ConvertDecToBCD(int dec);
    int         ConvertBCDtoDec(int bcd);

public slots:
    void        TCPconnected();
    void        TCPdisconnected();

signals:
    void        RequestJoinGame(int Game, int Tagger, int Flags);
    void        AckPlayerAssignment(int Game, int Tagger);
    void        TimerBlock(bool StartStop);
    void        sendSerialData(QByteArray dataToSend);
    void        TagSummaryReceived(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags);

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
    bool            dontAnnounceFailedSignal;
    bool            tcpCommsConnected;
    int             missedAnnounceCount;


    void            processPacket(QList<QByteArray> data);
    int             extract(QList<QByteArray> &data);
    void            findSerialPort();
    bool            isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum);
    bool            isCheckSumCorrect(int _command, int _game, int _tagsTaken, int _teamAndPlayer, int _survivedMinutes, int _survivedSeconds, int _zoneTimeMinutes, int _zoneTimeSeconds, int _flags, int _checksum);

    QString         createIRstring(int data);
};

extern LttoComms lttoComms;

#endif // LTTOCOMMS_H
