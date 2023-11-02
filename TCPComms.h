#ifndef TCPCOMMS_H
#define TCPCOMMS_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>

class TCPComms : public QObject
{
    Q_OBJECT

public:
	explicit	TCPComms(QObject *parent = nullptr);

	bool		ConnectTCP();
	bool		DisconnectTCP();
	bool		connectToCombobulatorWiFi();
	bool		checkIPaddress();

	bool		getIsTCPinitialised() const;
	void		setIsTCPinitialised(bool value);

public slots:
	void		connected();
	void		disconnected();
	void		receivePacket();
	void		sendPacket(QByteArray data);
	void		initialiseTCPsignalsAndSlots();

signals:
	void		newTCPdata(QByteArray dataReceived);
	void		TcpCommsConnectionStatus(bool status);

private:
	QTcpSocket	*tcpSocket;
	bool        isConnected;
	bool        isTCPinitialised;                   // used to trigger a single instance of TCPsocket connectSignals.
};

#endif // TCPCOMMS_H
