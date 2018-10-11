#include "TCPComms.h"
#include "LttoComms.h"
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>

//TCPComms tcpComms;

TCPComms::TCPComms(QObject *parent) :
	QObject(parent)
{
    qDebug() << "TCPComms::TCPComms() - Constructing.......";
    tcpSocket = new QTcpSocket(this);
	//lttoComms.setTcpCommsConnected(false);
	emit TcpCommsConnectionStatus(false);
    setIsTCPinitialised(false);
    isConnected = false;
}

bool TCPComms::ConnectTCP()
{
	connectToCombobulatorWiFi();


	qDebug() << "TCPComms::ConnectTCP() - Connecting";
	tcpSocket->connectToHost(HOST_IP_ADDRESS,TCP_IP_PORT);
	return true;
}

bool TCPComms::DisconnectTCP()
{
    tcpSocket->disconnectFromHost();
    qDebug() << "TCPComms::DisconnectTCP() - Disconnected !!!";
	//lttoComms.setTcpCommsConnected(false);
	emit TcpCommsConnectionStatus(false);
	return false;
}

bool TCPComms::connectToCombobulatorWiFi()
{
	return false;

	qDebug() << "TCPComms::connectToCombobulatorWiFi()";
	bool result = false;
	QNetworkConfiguration networkConfig;
	QNetworkConfigurationManager networkManager;
	auto networkConnection = networkManager.allConfigurations();

	for (auto &networkFound : networkConnection)
	{
		if (networkFound.bearerType() == QNetworkConfiguration::BearerWLAN)
		{
			qDebug() << "TCPComms::connectToCombobulatorWiFi() - Network found: " << networkFound.name();
			if (networkFound.name() == "Combobulator")
			networkConfig = networkFound;
			result = true;
		}
	}
	auto session = new QNetworkSession(networkConfig, this);
	session->open();
	qDebug() << "TCPComms::connectToCombobulatorWiFi() - " << result;
	return result;
}

void TCPComms::connected()
{
    qDebug() << "\n\tTCPComms::connected !!!!        :-)\n";
	emit TcpCommsConnectionStatus(true);
	qDebug() << "TCPComms::connected() - \tCONNECTION ESTABLISHED";
    isConnected = true;
}

void TCPComms::disconnected()
{
    qDebug() << "\n\t\tTCPComms::disconnected :-(\n";
	//lttoComms.setTcpCommsConnected(false);
	emit TcpCommsConnectionStatus(false);
    isConnected = false;
}

void TCPComms::receivePacket()
{
    QByteArray rX;
    rX = tcpSocket->readAll();
	emit newTCPdata(rX);
}

void TCPComms::sendPacket(QByteArray data)
{
	if (tcpSocket->state() == QAbstractSocket::ConnectedState)
    {
		//qDebug() << "TCPComms::sendPacket(QByteArray data)" << data;
		tcpSocket->write(data);
    }
	else
	{
	//TODO1:  FIX THIS ASAP		if(lttoComms.getSerialUSBcommsConnected() == false)
		//qDebug() << "TCPComms::sendPacket() - Triggered ConnectTCP()";
		ConnectTCP();
	}
}

void TCPComms::initialiseTCPsignalsAndSlots()
{
    qDebug() << "TCPcomms::initialiseTCPsignalsAndSlots() - Triggered";

    //Needs to be done here not in constructor because lttoComms a static and may not exist when this static is created.
    setIsTCPinitialised(true);
    connect(tcpSocket,     SIGNAL(connected()),                this,       SLOT(connected()) );
    connect(tcpSocket,     SIGNAL(disconnected()),             this,       SLOT(disconnected()) );
	connect(tcpSocket,     SIGNAL(readyRead()),                this,       SLOT(receivePacket()), Qt::DirectConnection );
	//connect(tcpSocket,     SIGNAL(disconnected()),             lttoComms, SLOT(TCPdisconnected()) );
	//connect(tcpSocket,     SIGNAL(connected()),                lttoComms, SLOT(TCPconnected()) );
	//connect(lttoComms,    SIGNAL(sendSerialData(QByteArray)), this,       SLOT(sendPacket(QByteArray)) );
	//connect(this,          SIGNAL(newTCPdata(QByteArray)),     lttoComms, SLOT(receivePacket(QByteArray)) );
}

bool TCPComms::getIsTCPinitialised() const
{
    return isTCPinitialised;
}

void TCPComms::setIsTCPinitialised(bool value)
{
    isTCPinitialised = value;
}
