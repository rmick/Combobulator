#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H

#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include "LazerSwarm.h"
#include "Defines.h"

class SerialComms : public QObject
{
    Q_OBJECT

    struct RxPacket;

public:
    explicit SerialComms(QObject *parent = 0);

    bool        sendPacket(char type, QString data = 0);
    void        testSerialPort();
    bool        useLazerSwarm;
    void        setUpSerialPort();
    void        closeSerialPort();
    int         getRxPacket();

signals:
    void    RequestJoinGame(int Game, int Tagger, int Flags);
    void    AckPlayerAssignment(int Game, int Tagger);
    void    SerialPortFound(QString portDetails);
    void    TimerBlock(bool StartStop);
    void    sendSerialData(QByteArray dataToSend);

private slots:
    void        receivePacket();

private:
    //QSerialPortInfo portToUse[25];
    QSerialPort     *serialUSB;

    int             calculatedCheckSumTx;
    int             calculatedCheckSumRx;

    QByteArray      irDataIn;
    QByteArrayList  rxPacketList;
    void            processPacket(QList<QByteArray> data);
    int             extract(QList<QByteArray> &data);
    void            findSerialPort();
    bool            isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum);
    int             ConvertDecToBCD(int dec);
    int             ConvertBCDtoDec(int bcd);

    struct          RxPacket
    {
        char command;
        int  data;
    } rxPacket;
};

extern SerialComms serialComms;

#endif // SERIALCOMMS_H
