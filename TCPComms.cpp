#include "TCPComms.h"
#include "LttoComms.h"

TCPComms tcpComms;

TCPComms::TCPComms(QObject *parent) :
    QObject(parent)
{
    //qDebug() << "TCPComms::TCPComms() - Constructing.......";
    tcpSocket = new QTcpSocket(this);
    setIsTCPinitialised(false);
    lttoComms.setTcpCommsConnected(false);
    isConnected = false;
    //DisconnectTCP();
}

bool TCPComms::ConnectTCP()
{
    //qDebug() << "Connecting...";

    tcpSocket->connectToHost(HOST_IP_ADDRESS,TCP_IP_PORT);

    if(!tcpSocket->waitForConnected(1000))
    {
       qDebug() << "Error: " <<  tcpSocket->errorString();
       return false;
    }
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
    isConnected = true;
    lttoComms.setTcpCommsConnected(true);
}

void TCPComms::disconnected()
{
    qDebug() << "\n\t\tTCPComms::disconnected :-(\n";
    isConnected = false;
    lttoComms.setTcpCommsConnected(false);
}

void TCPComms::bytesWritten (qint64 bytes)
{
    return;
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

    //TODO:
    // Bypassed for Debug only
return;
qDebug() << "TCPComms::sendPacket() - bypassed for debug !!!";
    // TODO: Remove the lines above

    if (tcpSocket->state() == QAbstractSocket::ConnectedState  )
    {
        tcpSocket->write(data);
        //qDebug() << "          TCPComms::sendPacket() - " + data;
    }
    else
    {
        ConnectTCP();
        //qDebug() << "TCPComms::sendPacket() -  TCP not connected: ";
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
    connect(tcpSocket,     SIGNAL(bytesWritten(qint64)),       this,       SLOT(bytesWritten(qint64)) );

    connect(&lttoComms,    SIGNAL(sendSerialData(QByteArray)), this,       SLOT(sendPacket(QByteArray)) );
    connect(this,          SIGNAL(newTCPdata(QByteArray)),     &lttoComms, SLOT(receivePacket(QByteArray)) );

    if (ConnectTCP())
    {
        qDebug() << "TCPComms::initialiseTCPsignalsAndSlots() - Connected :-)";
    }
    else
    {
        qDebug() << "TCPComms::initialiseTCPsignalsAndSlots() - NO CONNECTION !!!!!";
    }

}

bool TCPComms::getIsTCPinitialised() const
{
    return isTCPinitialised;
}

void TCPComms::setIsTCPinitialised(bool value)
{
    isTCPinitialised = value;
}
