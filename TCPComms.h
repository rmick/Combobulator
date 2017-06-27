#ifndef TCPCOMMS_H
#define TCPCOMMS_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include "Defines.h"

namespace Ui {
class TCPComms;
}

class TCPComms : public QObject
{
    Q_OBJECT

public:
    explicit TCPComms(QObject *parent = 0);

public slots:
    void connected();
    void disconnected();
    void bytesWritten (qint64 bytes);
    void readyRead();
    void sendData(QByteArray data);

signals:
    void newTCPdata(QByteArray dataReceived);

private slots:

private:
    //Ui::TCPComms    *ui;
    QTcpSocket      *socket;

    bool            isConnected;
};

extern TCPComms tcpComms;

#endif // TCPCOMMS_H
