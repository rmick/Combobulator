#ifndef TCPCOMMS_H
#define TCPCOMMS_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "Defines.h"

class TCPComms : public QObject
{
    Q_OBJECT

public:
    explicit TCPComms(QObject *parent = 0);

    bool ConnectTCP();
    bool DisconnectTCP();

    bool getIsTCPinitialised() const;
    void setIsTCPinitialised(bool value);

public slots:
    void connected();
    void disconnected();
    void bytesWritten (qint64 bytes);
    void receivePacket();
    void sendPacket(QByteArray data);
    void initialiseTCPsignalsAndSlots();

signals:
    void newTCPdata(QByteArray dataReceived);

private:
    QTcpSocket      *tcpSocket;
    bool            isConnected;
    bool            isTCPinitialised;                   // used to trigger a single instance of TCPsocket connectSignals.
};

extern TCPComms tcpComms;

#endif // TCPCOMMS_H
