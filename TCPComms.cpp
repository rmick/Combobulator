#include "TCPComms.h"
#include "LttoComms.h"

TCPComms::TCPComms(QObject *parent) :
    QObject(parent)
{
    tcpSocket = new QTcpSocket(this);
    isConnected = false;

    connect(tcpSocket,     SIGNAL(connected()),                this,       SLOT(connected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             this,       SLOT(disconnected()) );
    connect(tcpSocket,     SIGNAL(readyRead()),                this,       SLOT(receivePacket()) );
    connect(tcpSocket,     SIGNAL(bytesWritten(qint64)),       this,       SLOT(bytesWritten(qint64)) );

    connect(&lttoComms,    SIGNAL(sendSerialData(QByteArray)), this,       SLOT(sendPacket(QByteArray)) );
    connect(this,          SIGNAL(newTCPdata(QByteArray)),     &lttoComms, SLOT(receivePacket(QByteArray)) );

    tcpSocket->connectToHost("192.168.2.1",8000);
}

void TCPComms::connected()
{
    qDebug() << "\n\tTCPComms::connected !!!!        :-)\n";
    isConnected = true;
}

void TCPComms::disconnected()
{
    qDebug() << "TCPComms::disconnected :-(";
    isConnected = false;
}

void TCPComms::bytesWritten (qint64 bytes)
{
   if(bytes == 0) qDebug() << "Impossible";
}

void TCPComms::receivePacket()
{
    QByteArray rX;
    rX = tcpSocket->readAll();
    //emit newTCPdata(rX);
    lttoComms.androidRxPacket(rX);
    //qDebug() << "          TCPComms::receivePacket() - " + rX;
}

int retryCount = 0;
void TCPComms::sendPacket(QByteArray data)
{
    if (isConnected)
    {
        tcpSocket->write(data);
        //qDebug() << "          TCPComms::sendPacket() - " + data;
    }
    else
    {
        if (retryCount < 20)
        {
            retryCount++;
            return;
        }
        tcpSocket->connectToHost("192.168.2.1",8000);
        //qDebug() << "TCPComms::sendPacket() - Trying to connect to Socket......";
        retryCount= 0;
    }
}
