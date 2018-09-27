#ifndef LTTOCOMMS_H
#define LTTOCOMMS_H

#include <QObject>
#include <QTimer>
#include "Defines.h"
#include "LazerSwarm.h"

#include "SerialUSBcomms.h"
#include "TCPComms.h"

class LttoComms : public QObject
{
    Q_OBJECT

private:
	explicit    LttoComms(QObject *parent = 0);
	static		LttoComms *instance;

public:
	static		LttoComms *getInstance();

	void		initialise();

	bool		getConnectionStatus();

    bool        sendPacket(char type, int data = 0, bool dataFormat = false);
	void        sendLCDtext(QString textToSend, int lineNumber, bool drawScreen);
	void        sendLCDtext(int xCursor, int yCursor, QString text, int fontSize, int colour, bool clearDisp, bool drawDisplay);
	void		sendLEDcolour(int Red = 0, int Green = 0, int Blue = 0);
	void		sendLEDcolour(QString colour);
	void		sendOTAtext(QString ssidText, QString pswdText);
	void		sendPing(QString pingText);
	void		sendEspRestart();

    bool        getUseLazerSwarm() const;
    void        setUseLazerSwarm(bool value);

    bool        getDontAnnounceGame() const;
    void        setDontAnnounceGame(bool value);

    int         getMissedAnnounceCount() const;
    void        setMissedAnnounceCount(int value);

    void        androidRxPacket(QByteArray data);

    bool        getTcpCommsConnected() const;
//    void        setTcpCommsConnected(bool value);

    bool        getDontAnnounceFailedSignal() const;
    void        setDontAnnounceFailedSignal(bool value);

    void        nonBlockingDelay(int mSec);

    bool        getIsUSBinitialised() const;
    void        setIsUSBinitialised(bool value);

    int         ConvertDecToBCD(int dec);
    int         ConvertBCDtoDec(int bcd);

    bool        getSerialUSBcommsConnected() const;
    void        setSerialUSBcommsConnected(bool value);

    bool        getUseLongDataPacketsOverTCP() const;
    void        setUseLongDataPacketsOverTCP(bool value);

	void		closePorts();

public slots:
	void        setTcpCommsConnected(bool value);

signals:
    void        RequestJoinGame(int Game, int Tagger, int Flags, bool isLtar);
    void        AckPlayerAssignment(int Game, int Tagger, bool isLtar);
    void        TimerBlock(bool StartStop);
    void        sendSerialData(QByteArray dataToSend);
    void        TagSummaryReceived(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags);
    void        Team1TagReportReceived(int game, int teamAndPlayer, int tagsReceivedfromPlayer[]);
    void        Team1TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
    void        Team2TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
    void        Team3TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
	void		BeaconReceived(int beaconData);
	void		PongReceived(QString pongText);

private slots:
    void        receivePacket(QByteArray RxData);

private:

    int             calculatedCheckSumTx;
    int             calculatedCheckSumRx;
    QByteArray      irDataIn;
    QByteArrayList  rxPacketList;
    QString         packetString;
    bool            useLazerSwarm;
    bool            dontAnnounceGame;
    bool            dontAnnounceFailedSignal;
    bool            tcpCommsConnected;
    bool            serialUSBcommsConnected;
    int             missedAnnounceCount;
    bool            useLongDataPacketsOverTCP;
//    int             tagsReceivedfromPlayer[8];

	TCPComms		*tcpComms;
	SerialUSBcomms	*serialUSBcomms;
	LazerSwarm		*lazerSwarm;

    void            processPacket(QList<QByteArray> data);
    int             extract(QList<QByteArray> &data);
    void            findSerialPort();
    bool            isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum);
    bool            isCheckSumCorrect(int _command, int _game, int _tagsTaken, int _teamAndPlayer, int _survivedMinutes, int _survivedSeconds, int _zoneTimeMinutes, int _zoneTimeSeconds, int _flags, int _checksum);
    bool            isCheckSumCorrect(int _command, int _game, int _teamAndPlayer, int _playersInReportByte, int _tagsP1, int _tagsP2, int _tagsP3, int _tagsP4, int _tagsP5, int _tagsP6, int _tagsP7, int _tagsP8, int _checksum);
    QString         createIRstring(int data);
};

#endif // LTTOCOMMS_H
