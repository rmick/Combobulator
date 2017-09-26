#include "TCPComms.h"
#include "LttoComms.h"

TCPComms tcpComms;

TCPComms::TCPComms(QObject *parent) :
    QObject(parent)
{
    qDebug() << "TCPComms::TCPComms() - Constructing.......";
    tcpSocket = new QTcpSocket(this);
    lttoComms.setTcpCommsConnected(false);
    setIsTCPinitialised(false);
    isConnected = false;
}

bool TCPComms::ConnectTCP()
{
    tcpSocket->connectToHost(HOST_IP_ADDRESS,TCP_IP_PORT);
    return true;
}

bool TCPComms::DisconnectTCP()
{
    tcpSocket->disconnectFromHost();
    qDebug() << "TCPComms::DisconnectTCP() - Disconnected !!!";
    return false;
}

void TCPComms::connected()
{
    qDebug() << "\n\tTCPComms::connected !!!!        :-)\n";
    lttoComms.setTcpCommsConnected(true);
    isConnected = true;
}

void TCPComms::disconnected()
{
    qDebug() << "\n\t\tTCPComms::disconnected :-(\n";
    lttoComms.setTcpCommsConnected(false);
    isConnected = false;
}

void TCPComms::receivePacket()
{
    QByteArray rX;
    rX = tcpSocket->readAll();
    emit newTCPdata(rX);          // Does not work on Android for some unknown reason.
    //lttoComms.androidRxPacket(rX);  // This is the simple workaround :-)
}

void TCPComms::sendPacket(QByteArray data)
{
    if (tcpSocket->state() == QAbstractSocket::ConnectedState)
    {
        tcpSocket->write(data);
    }
    else
    {
        if(lttoComms.getSerialUSBcommsConnected() == false)  ConnectTCP();
    }
}

void TCPComms::initialiseTCPsignalsAndSlots()
{
    qDebug() << "TCPcomms::initialiseTCPsignalsAndSlots() - Triggered";

    //Needs to be done here not in constructor because lttoComms a static and may not exist when this static is created.
    setIsTCPinitialised(true);

    connect(tcpSocket,     SIGNAL(connected()),                this,       SLOT(connected()) );
    connect(tcpSocket,     SIGNAL(connected()),                &lttoComms, SLOT(TCPconnected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             this,       SLOT(disconnected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             &lttoComms, SLOT(TCPdisconnected()) );
    connect(tcpSocket,     SIGNAL(readyRead()),                this,       SLOT(receivePacket()) );

    connect(&lttoComms,    SIGNAL(sendSerialData(QByteArray)), this,       SLOT(sendPacket(QByteArray)) );
    connect(this,          SIGNAL(newTCPdata(QByteArray)),     &lttoComms, SLOT(receivePacket(QByteArray)) );
}

bool TCPComms::getIsTCPinitialised() const
{
    return isTCPinitialised;
}

void TCPComms::setIsTCPinitialised(bool value)
{
    isTCPinitialised = value;
}
