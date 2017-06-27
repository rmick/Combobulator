#include "TCPComms.h"
#include "LttoComms.h"

TCPComms tcpComms;

TCPComms::TCPComms(QObject *parent) :
    QObject(parent)
{
    socket = new QTcpSocket(this);
    isConnected = false;

    connect(socket,     SIGNAL(connected()),                this, SLOT(connected()));
    connect(socket,     SIGNAL(disconnected()),             this, SLOT(disconnected()));
    connect(socket,     SIGNAL(readyRead()),                this, SLOT(readyRead()));
    connect(socket,     SIGNAL(bytesWritten(qint64)),       this, SLOT(bytesWritten(qint64)));
  //connect(&lttoComms, SIGNAL(sendSerialData(QByteArray)), this, SLOT(sendData(QByteArray)));
  //  connect(this,       SIGNAL(newTCPdata(QByteArray)),     &lttoComms, SLOT(receivePacket(QByteArray)) );

    socket->connectToHost("192.168.2.1",8000);
    qDebug() << "TCP Connecting to LTTO_host";
}

void TCPComms::connected()
{
    qDebug() << "TCP Connected :-)";
    isConnected = true;
}

void TCPComms::disconnected()
{
    qDebug() << "TCP Disconnected :-(";
    isConnected = false;
}

void TCPComms::bytesWritten (qint64 bytes)
{
    qDebug() << "TCP Bytes Written = " << bytes;
}

void TCPComms::readyRead()
{
    QByteArray rX;
    rX = socket->readAll();
    qDebug() << rX;
    emit newTCPdata(rX);
    qDebug() << "Reading TCP: " + rX;
}

void TCPComms::sendData(QByteArray data)
{
    if (socket->isOpen() )
    {
        socket->write(data);
        qDebug() << "Data Sent: " + data;
    }
}
