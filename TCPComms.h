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
};

extern TCPComms tcpComms;

#endif // TCPCOMMS_H
