#include "TCPComms.h"
#include "LttoComms.h"

TCPComms::TCPComms(QObject *parent) :
    QObject(parent)
{
    qDebug() << "TCPComms::TCPComms()";
    tcpSocket = new QTcpSocket(this);
    isConnected = false;

    connect(tcpSocket,     SIGNAL(connected()),                this,       SLOT(connected()) );
    connect(tcpSocket,     SIGNAL(connected()),                &lttoComms, SLOT(TCPconnected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             this,       SLOT(disconnected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             &lttoComms, SLOT(TCPdisconnected()) );
    connect(tcpSocket,     SIGNAL(readyRead()),                this,       SLOT(receivePacket()) );
    connect(tcpSocket,     SIGNAL(bytesWritten(qint64)),       this,       SLOT(bytesWritten(qint64)) );

    connect(&lttoComms,    SIGNAL(sendSerialData(QByteArray)), this,       SLOT(sendPacket(QByteArray)) );
    connect(this,          SIGNAL(newTCPdata(QByteArray)),     &lttoComms, SLOT(receivePacket(QByteArray)) );

    tcpSocket->connectToHost(HOST_IP_ADDRESS, 8000);
}

void TCPComms::connected()
{
    qDebug() << "\n\tTCPComms::connected !!!!        :-)\n";
    isConnected = true;
    lttoComms.setTcpCommsConnected(true);
}

void TCPComms::disconnected()
{
    qDebug() << "TCPComms::disconnected :-(";
    isConnected = false;
    lttoComms.setTcpCommsConnected(false);
}

void TCPComms::bytesWritten (qint64 bytes)
{
   if(bytes > 0) qDebug() << "TCPComms::bytesWritten()";
}

void TCPComms::receivePacket()
{
    QByteArray rX;
    rX = tcpSocket->readAll();
    emit newTCPdata(rX);          // Does not work on Android for some unknown reason.
    //lttoComms.androidRxPacket(rX);  // This is the simple workaround :-)
    qDebug() << "          TCPComms::receivePacket() - " + rX;
}

int retryCount = 0;
void TCPComms::sendPacket(QByteArray data)
{
    if (isConnected)
    {
        tcpSocket->write(data);
        qDebug() << "          TCPComms::sendPacket() - " + data;
    }
    else
    {
        if (retryCount < 30)
        {
            retryCount++;
            return;
        }
        tcpSocket->connectToHost(HOST_IP_ADDRESS, 8000);
        qDebug() << "TCPComms::sendPacket() - Trying to connect to Socket......";
        retryCount= 0;
    }
}

void TCPComms::initialiseTCPsignalsAndSlots()
{
    return;

    //Needs to be done here not in constructor because lttoComms a static and may not exist when this static is created.
    qDebug() << "TCPcomms::initialiseTCPsignalsAndSlots() - Triggered";
    connect(tcpSocket,     SIGNAL(connected()),                this,       SLOT(connected()) );
    connect(tcpSocket,     SIGNAL(connected()),                &lttoComms, SLOT(TCPconnected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             this,       SLOT(disconnected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             &lttoComms, SLOT(TCPdisconnected()) );
    connect(tcpSocket,     SIGNAL(readyRead()),                this,       SLOT(receivePacket()) );
    connect(tcpSocket,     SIGNAL(bytesWritten(qint64)),       this,       SLOT(bytesWritten(qint64)) );

    connect(&lttoComms,    SIGNAL(sendSerialData(QByteArray)), this,       SLOT(sendPacket(QByteArray)) );
    connect(this,          SIGNAL(newTCPdata(QByteArray)),     &lttoComms, SLOT(receivePacket(QByteArray)) );

    tcpSocket->connectToHost(HOST_IP_ADDRESS, 8000);
}
