#include "LttoComms.h"
#include <QEventLoop>
#include <QDebug>
#include "Game.h"

//#define LOCAL_DEBUG_RX
//#define LOCAL_DEBUG_TX
//#define LCD_DISABLED

LttoComms lttoComms;

LttoComms::LttoComms(QObject *parent) : QObject(parent)
{
    qDebug() << "LttoComms::LttoComms() - Constructing.......";
    dontAnnounceGame = false;
    useLongDataPacketsOverTCP = false;
}

////////////////////////////////////////////////////////////////////

bool LttoComms::sendPacket(char type, int data, bool dataFormat)
{
    bool result = true;
    bool fullTCPpacketToSend = false;
    static QByteArray packet;

    //Calculating BCD and the CheckSum.
    //This is here because it always does my head in !!!!!
    //Certain Data is BCD Hex                               It means it is 2 x 4bit numbers in a single Byte
    //                                                      Therefore 10 is actually 0001 0000 (which is 0x16)
    //                                                      and       42 is actually 0100 0010 (which is 0x64)
    //                                                      and       79 is actually 0111 1001 (which is 0x121)
    //                                                      and      100 becomes     1111 1111 (which is 0xFF)
    //This means that '10' is actually 0x16.
    //However other data (taggerID, GameID, Flags, etc) are just plain Hex/Dec all the time.
    //****** Now comes the nasty part!!!!! ******
    //The LazerSwarm then needs all data sent as actual Hex, whereas the LTTO library expects it as Dec '10'.
    //The CheckSum is the Sum of the final Hex values or Decimal values AFTER conversion (where applicable) to BCD.

    switch (type)
    {
        case PACKET:
            calculatedCheckSumTx = data;
            fullTCPpacketToSend = false;
            packetString = createIRstring(data);
            packetString.prepend('P');
            if(lttoComms.getUseLongDataPacketsOverTCP()) packetString.prepend("ltto:");
            packet.append(packetString);
            //qDebug() << "lttoComms::sendPacket() - Packet =    " << data << "\t:" << packetString;
            break;
        case DATA:
            if (dataFormat == BCD)
            {
                calculatedCheckSumTx += ConvertDecToBCD(data);
                data = ConvertDecToBCD(data);
                packetString = createIRstring(data);
                packetString.prepend('D');
                packet.append(packetString);
                //qDebug() << "lttoComms::sendPacket() - BCD Data = " << data << "\t:" << packetString << "\tBCD=" << dataFormat;
            }
            else
            {
                calculatedCheckSumTx += data;
                packetString = createIRstring(data);
                packetString.prepend('D');
                packet.append(packetString);
                //qDebug() << "lttoComms::sendPacket() - Dec Data = " << data << "\t:" << packetString << "\tBCD=" << dataFormat;
            }
            break;
        case CHECKSUM:
            calculatedCheckSumTx = calculatedCheckSumTx % 256;      // CheckSum is the remainder of dividing by 256.
            calculatedCheckSumTx = calculatedCheckSumTx | 256;      // Set the required 9th MSB bit to 1 to indicate it is a checksum
            packetString = createIRstring(calculatedCheckSumTx);
            packetString.prepend('C');
            packet.append(packetString);
            fullTCPpacketToSend = true;
            //qDebug() << "lttoComms::sendPacket() - CheckSum = \t" << packetString;
            break;
        case TAG:
            packetString = createIRstring(data);
            packetString.prepend('T');
            if(lttoComms.getUseLongDataPacketsOverTCP()) packetString.prepend("ltto:");
            packet.append(packetString);
            fullTCPpacketToSend = true;
            //qDebug() << "lttoComms::sendPacket() - Tag = \t" << packetString << endl << endl;
            break;
        case BEACON:
            packetString = createIRstring(data);
            packetString.prepend('B');
            if(lttoComms.getUseLongDataPacketsOverTCP()) packetString.prepend("ltto:");
            packet.append(packetString);
            fullTCPpacketToSend = true;
            break;
        default:
            qDebug() << "lttoComms::sendPacket() - No Packet type specified. You ninkom poop.";
            result = false;
    }


    if (useLongDataPacketsOverTCP)
    {
        packet.append(":");
        if (fullTCPpacketToSend == true)
        {
            packet.append(" \r\n");
            emit sendSerialData(packet);
            #ifdef LOCAL_DEBUG_TX
                qDebug() << "lttoComms::sendPacket() - WIFIKIT32 - Full Packet = " << packet;
            #endif
            fullTCPpacketToSend = false;
            packet.clear();
        }
    }
    else
    {
        if (useLazerSwarm)
        {
            QByteArray packetToTranslate;
            packetToTranslate.append(packet);
            packet.clear();
            packet.append(lazerswarm.translateCommand(packetToTranslate) );
            packet.append(" \r\n");
        }
        else packet.append(":");
        //qDebug() << "lttoComms::sendPacket() LazerSwarm Mode- " << packet;
        emit sendSerialData(packet);            //Connects to TCPComms::sendPacket slot && SerialUSBcomms::sendPacket slot
        tcpComms.sendPacket(packet);
        packet.clear();
        nonBlockingDelay(INTERPACKET_DELAY_MSEC);
    }

    return result;
}

void LttoComms::sendLCDtext(QString textToSend, int lineNumber)
{

#ifdef LCD_DISABLED
    return;
#endif

    if(lttoComms.getSerialUSBcommsConnected()) return;      // USB means it is a Lazerswarm, which does not accept TXT.
    QByteArray textBA;
    textToSend.prepend("TXT" + QString::number(lineNumber)+ ":");
    textBA.append(textToSend + "\r\n");
    //qDebug() << "LttoComms::sendLCDtext:" << textBA;
    //emit sendSerialData(textBA);
    tcpComms.sendPacket(textBA);
    //lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
}

void LttoComms::sendLCDtext(int xCursor, int yCursor, QString text, int fontSize, int colour, bool clearDisp)
{

#ifdef LCD_DISABLED
    return;
#endif

    if(lttoComms.getSerialUSBcommsConnected()) return;      // USB means it is a Lazerswarm, which does not accept TXT.
    QByteArray textBA;
    QString textToSend = "";
    textToSend.prepend("DSP," + QString::number(xCursor) + "," + QString::number(yCursor) + "," + text + "," +  QString::number(fontSize) + "," + QString::number(colour) + "," + QString::number(clearDisp));
    textBA.append(textToSend + "\r\n");
    //qDebug() << "LttoComms::sendLCDtext(DSPmode): " << textBA;
    tcpComms.sendPacket(textBA);
    //emit sendSerialData(textBA);
    //lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
}

QString LttoComms::createIRstring(int data)
{
    QString createdPacket;
    if (useLazerSwarm)  createdPacket = QString::number(data,16).toUpper();
    else                createdPacket = QString::number(data,10);
    if (createdPacket.length() == 1) createdPacket.prepend('0');
    return createdPacket;
}

////////////////////////////////////////////////////////////////////

void LttoComms::receivePacket(QByteArray RxData)
{
    bool isPacketComplete = false;

    //qDebug() << "\tLttoComms::receivePacket() -"<< RxData;

    irDataIn.append(RxData);

    if (irDataIn.endsWith("\r\n"))
    {
        //qDebug() << "LttoComms::receivePacket()" << irDataIn;

        //Remove the \r\n
        irDataIn = irDataIn.trimmed();
        //irDataIn = irDataIn.trimmed();  //there is a bug in the ESP32 code that sometimes sends a double \r\n

       //Check for an Error message
        if      (irDataIn.startsWith("ERROR"))
        {
            qDebug() <<"LttoComms::receivePacket() Error:" << irDataIn;
            irDataIn.clear();
            return;
        }

        else if (irDataIn.startsWith("STOP"))
        {
            setDontAnnounceGame(true);
            qDebug() << "STOP - LttoComms::receivePacket - ESP is receiving, so lets be quiet please.";
            irDataIn.clear();
            return;
        }

        else if (irDataIn.startsWith("START"))
        {
            setDontAnnounceGame(false);
            setDontAnnounceFailedSignal(false);
            qDebug() << "LttoComms::receivePacket - Resuming transmission.";
            irDataIn.clear();
            return;
        }

        //Check if this is a CombobulatorHost or LazerSwarm packet.
        else if (irDataIn.endsWith("@"))
        // CombobulatorHost packet
        {
            setDontAnnounceGame(true);
            setDontAnnounceFailedSignal(true);
            // remove the @
            irDataIn.remove((irDataIn.size()-1), 1);

            //BreakUp messages into "Pxx", "Dxx", "Dxx", "Cxxx,"
            int delimiter = 0;
            QByteArray packetToAdd;
            while(irDataIn.length() > 0)
            {
                delimiter = irDataIn.indexOf(",");
                packetToAdd = irDataIn.left(delimiter);
                irDataIn.remove(0, delimiter + 1);
                //qDebug() << "** - " << irDataIn;
                rxPacketList.append(packetToAdd);
            }
#ifdef LOCAL_DEBUG_RX
            qDebug() << "LttoComms::receivePacket() -> LTTO library mode - " << " - " << rxPacketList;
#endif
            isPacketComplete = true;
        }
        else if (irDataIn.startsWith("RCV"))
        // Lazerswarm packet
        {
            rxPacketList.append(lazerswarm.decodeCommand(irDataIn));
           #ifdef LOCAL_DEBUG_RX
                qDebug() << "LttoComms::receivePacket() LazerSwarm mode - " << lazerswarm.decodeCommand(irDataIn);
            #endif
            if (lazerswarm.decodeCommand(irDataIn).startsWith("C"))
            {
            #ifdef LOCAL_DEBUG_RX
                qDebug() << "LttoComms::receivePacket() LazerSwarm mode -   ___________";
            #endif
                isPacketComplete = true;
            }
            irDataIn = "";
        }

        //TODO: Somewhere here I should process Beacons so I can be hostile, and Tags to know which team to be hostile towards.
        //      Or do I do that inside the WiFiKit32 ??????

        else
        {
            //The message is invalid, so start Tx again.
            qDebug() << "\t\tLttoComms::receivePacket() - bad packet receveived, bailing out" << irDataIn;
            lttoComms.setDontAnnounceGame(false);
            irDataIn="";
        }
    }

    if (isPacketComplete)
    {
        irDataIn.clear();
        if (!rxPacketList.empty())
        {
            //qDebug() << "isPacketComplete:" << rxPacketList.last();
            //if (!rxPacketList.last().startsWith("D") || !rxPacketList.first().startsWith("P"))
//            if (rxPacketList.last().startsWith("C") || rxPacketList.first().startsWith("B") || rxPacketList.first().startsWith("T") )
//            {
              #ifdef LOCAL_DEBUG_RX
                qDebug() << "\tLttoComms::receivePacket()  - FULL PACKET!" << rxPacketList;
              #endif
                processPacket(rxPacketList);
//            }
        }
    }
}

bool LttoComms::getUseLongDataPacketsOverTCP() const
{
    return useLongDataPacketsOverTCP;
}

void LttoComms::setUseLongDataPacketsOverTCP(bool value)
{
    useLongDataPacketsOverTCP = value;
}

bool LttoComms::getSerialUSBcommsConnected() const
{
    return serialUSBcommsConnected;
}

void LttoComms::setSerialUSBcommsConnected(bool value)
{
    serialUSBcommsConnected = value;
}

bool LttoComms::getDontAnnounceFailedSignal() const
{
    return dontAnnounceFailedSignal;
}

void LttoComms::setDontAnnounceFailedSignal(bool value)
{
    dontAnnounceFailedSignal = value;
}

/////////////////////////////////////////////////////////////////////////

bool LttoComms::getTcpCommsConnected() const
{
    return tcpCommsConnected;
}

void LttoComms::setTcpCommsConnected(bool value)
{
    tcpCommsConnected = value;
    setUseLongDataPacketsOverTCP(value);                        // Activated/Deactivated here !!!!
    lttoComms.setUseLazerSwarm(!value);
}

void LttoComms::TCPconnected()
{
    tcpCommsConnected = true;
}

void LttoComms::TCPdisconnected()
{
    tcpCommsConnected = false;
}

/////////////////////////////////////////////////////////////////////////

int LttoComms::getMissedAnnounceCount() const
{
    return missedAnnounceCount;
}

void LttoComms::setMissedAnnounceCount(int value)
{
    missedAnnounceCount = value;
}

void LttoComms::androidRxPacket(QByteArray data)
{
    receivePacket(data);
}

bool LttoComms::getDontAnnounceGame() const
{
    return dontAnnounceGame;
}

void LttoComms::setDontAnnounceGame(bool value)
{
    dontAnnounceGame = value;
    if (value == true) missedAnnounceCount = 0;
}

bool LttoComms::getUseLazerSwarm() const
{
    return useLazerSwarm;
}

void LttoComms::setUseLazerSwarm(bool value)
{
    qDebug() <<"LttoComms::setUseLazerSwarm = " << value;
    useLazerSwarm = value;
}

bool LttoComms::isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum)  // Normal Hosting
{
    bool result = false;
    calculatedCheckSumRx =  _command;
    calculatedCheckSumRx += _game;
    calculatedCheckSumRx += _tagger;
    calculatedCheckSumRx += _flags;
    calculatedCheckSumRx = calculatedCheckSumRx%256;
    if (calculatedCheckSumRx == _checksum%256)  result = true;
    return result;
}

bool LttoComms::isCheckSumCorrect(int _command, int _game, int _teamAndPlayer, int _tagsTaken, int _survivedMinutes, int _survivedSeconds, int _zoneTimeMinutes, int _zoneTimeSeconds, int _flags, int _checksum)       // Debrief mode
{
    bool result = false;
    calculatedCheckSumRx =  _command;
    calculatedCheckSumRx += _game;
    calculatedCheckSumRx += _teamAndPlayer;
    calculatedCheckSumRx += _tagsTaken;
    calculatedCheckSumRx += _survivedMinutes;
    calculatedCheckSumRx += _survivedSeconds;
    calculatedCheckSumRx += _zoneTimeMinutes;
    calculatedCheckSumRx += _zoneTimeSeconds;
    calculatedCheckSumRx += _flags;
    calculatedCheckSumRx = calculatedCheckSumRx%256;
    if (calculatedCheckSumRx == _checksum%256)  result = true;
    return result;
}

bool LttoComms::isCheckSumCorrect(int _command, int _game, int _teamAndPlayer, int _playersInReportByte, int _tagsP1, int _tagsP2, int _tagsP3, int _tagsP4, int _tagsP5, int _tagsP6, int _tagsP7, int _tagsP8, int _checksum)
{
    bool result = false;
    calculatedCheckSumRx =  _command;
    calculatedCheckSumRx += _game;
    calculatedCheckSumRx += _teamAndPlayer;
    calculatedCheckSumRx += _playersInReportByte;
    calculatedCheckSumRx += _tagsP1;
    calculatedCheckSumRx += _tagsP2;
    calculatedCheckSumRx += _tagsP3;
    calculatedCheckSumRx += _tagsP4;
    calculatedCheckSumRx += _tagsP5;
    calculatedCheckSumRx += _tagsP6;
    calculatedCheckSumRx += _tagsP7;
    calculatedCheckSumRx += _tagsP8;
    calculatedCheckSumRx = calculatedCheckSumRx%256;
    if (calculatedCheckSumRx == _checksum%256)  result = true;
    return result;
}

void LttoComms::processPacket(QList<QByteArray> data)
{
    //#ifdef LOCAL_DEBUG_RX
        qDebug() << "\tLttoComms::processPacket()" << data;
    //#endif

    int command = extract(data);
    int game                    = 0;
    int tagger                  = 0;
    int taggerInfo              = 0;
    int smartDeviceInfo         = 0;
    int flags                   = 0;
    int teamAndPlayer           = 0;
    int tagsTaken               = 0;
    int playersInReportByte     = 0;
    int survivedMinutes         = 0;
    int survivedSeconds         = 0;
    int zoneTimeMinutes         = 0;
    int zoneTimeSeconds         = 0;
    int checksum                = 0;

    int tagsP1 = 0;
    int tagsP2 = 0;
    int tagsP3 = 0;
    int tagsP4 = 0;
    int tagsP5 = 0;
    int tagsP6 = 0;
    int tagsP7 = 0;
    int tagsP8 = 0;

    switch (command)
    {
    case REQUEST_JOIN_GAME:         //P16
        game     = extract(data);
        tagger   = extract(data);
        flags    = extract(data);
        checksum = extract(data);
        if(isCheckSumCorrect(command, game, tagger, flags, checksum))   emit RequestJoinGame(game, tagger, flags, false);
        //else                                                            setDontAnnounceGame(false);
        break;

    case REQUEST_JOIN_LTAR_GAME:
        game            = extract(data);
        tagger          = extract(data);
        taggerInfo      = extract(data);
        smartDeviceInfo = extract(data);
        checksum        = extract(data);
        //TODO: Fix the Checksum that is not working - posibly due to DEC not BCD ??
        //if(isCheckSumCorrect(command, game, tagger, taggerInfo, smartDeviceInfo, checksum)) emit RequestJoinGame(game, tagger, 0, true);
        //else                                                                                setDontAnnounceGame(false);
             emit RequestJoinGame(game, tagger, 0, true);
        break;

    case ACK_PLAYER_ASSIGN:         //P17
        game     = extract(data);
        tagger   = extract(data);
        checksum = extract(data);
        if(isCheckSumCorrect(command, game, tagger, flags, checksum))   emit AckPlayerAssignment(game, tagger, false);
        else                                                            setDontAnnounceFailedSignal(true);
        break;

    case ACK_LTAR_PLAYER_ASSIGN:
        game     = extract(data);
        tagger   = extract(data);
        checksum = extract(data);
        //TODO: Fix the Checksum that is not working
        //if(isCheckSumCorrect(command, game, tagger, flags, checksum)) emit AckPlayerAssignment(game, tagger, true);
        //else                                                          setDontAnnounceGame(false);
        //break;

        emit AckPlayerAssignment(game, tagger, true);
        break;

    case TAG_SUMMARY:
        game                = extract(data);
        teamAndPlayer       = extract(data);
        tagsTaken           = extract(data);
        survivedMinutes     = extract(data);
        survivedSeconds     = extract(data);
        zoneTimeMinutes     = extract(data);
        zoneTimeSeconds     = extract(data);
        flags               = extract(data);
        checksum            = extract(data);

        if(isCheckSumCorrect(command, game, teamAndPlayer, tagsTaken, survivedMinutes, survivedSeconds, zoneTimeMinutes, zoneTimeSeconds, flags, checksum))
                emit TagSummaryReceived(game, teamAndPlayer, tagsTaken, survivedMinutes, survivedSeconds, zoneTimeMinutes, zoneTimeSeconds, flags);
        else    setDontAnnounceGame(false);
        break;

    case TEAM_1_TAG_REPORT:
    case TEAM_2_TAG_REPORT:
    case TEAM_3_TAG_REPORT:
        game                = extract(data);
        teamAndPlayer       = extract(data);
        playersInReportByte = extract(data);

        if((playersInReportByte >> 0) & 1)  tagsP1 = extract(data);
        if((playersInReportByte >> 1) & 1)  tagsP2 = extract(data);
        if((playersInReportByte >> 2) & 1)  tagsP3 = extract(data);
        if((playersInReportByte >> 3) & 1)  tagsP4 = extract(data);
        if((playersInReportByte >> 4) & 1)  tagsP5 = extract(data);
        if((playersInReportByte >> 5) & 1)  tagsP6 = extract(data);
        if((playersInReportByte >> 6) & 1)  tagsP7 = extract(data);
        if((playersInReportByte >> 7) & 1)  tagsP8 = extract(data);
        checksum            = extract(data);

        if(isCheckSumCorrect(command, game, teamAndPlayer, playersInReportByte, tagsP1, tagsP2, tagsP3, tagsP4, tagsP5, tagsP6, tagsP7, tagsP8, checksum))
        {
            tagsP1 = ConvertBCDtoDec(tagsP1);
            tagsP2 = ConvertBCDtoDec(tagsP2);
            tagsP3 = ConvertBCDtoDec(tagsP3);
            tagsP4 = ConvertBCDtoDec(tagsP4);
            tagsP5 = ConvertBCDtoDec(tagsP5);
            tagsP6 = ConvertBCDtoDec(tagsP6);
            tagsP7 = ConvertBCDtoDec(tagsP7);
            tagsP8 = ConvertBCDtoDec(tagsP8);

            switch(command)
            {
            case TEAM_1_TAG_REPORT:
                emit Team1TagReportReceived(game, teamAndPlayer, tagsP1, tagsP2, tagsP3, tagsP4, tagsP5, tagsP6, tagsP7, tagsP8);
                break;
            case TEAM_2_TAG_REPORT:
                emit Team2TagReportReceived(game, teamAndPlayer, tagsP1, tagsP2, tagsP3, tagsP4, tagsP5, tagsP6, tagsP7, tagsP8);
                break;
            case TEAM_3_TAG_REPORT:
                emit Team3TagReportReceived(game, teamAndPlayer, tagsP1, tagsP2, tagsP3, tagsP4, tagsP5, tagsP6, tagsP7, tagsP8);
                break;
            }
        }
        else    setDontAnnounceGame(false);
        break;

    //TODO: BEACON and TEAM_@_TAG_REPORT both have a value of 0x66, so this barks!!!
    //case BEACON:
        //TODO: Add some code to action Beacons
        break;

    case TAG:
        break;

    default:
        //We end up here if the ESP32 misses the Packet Header
        qDebug() << "LttoComms::processPacket() - How did we get here ????";
        break;

    }
    rxPacketList.clear();
    data.clear();
}

int LttoComms::extract(QList<QByteArray> &data)
{
    QString command;
    if (data.empty() == false)
    {
        command = data.takeFirst();
        command = command.mid(1 , (command.length()-1) );
    }
    bool ok = false;
    return command.toInt(&ok, 10);
}

int LttoComms::ConvertDecToBCD(int dec)
{
    if(gameInfo.getIsLTARGame())    return dec;     //LTAR hosting protocol does not use BCD

    if (dec == 100) return 0xFF;
    return (int) (((dec/10) << 4) | (dec %10) );
}

int LttoComms::ConvertBCDtoDec(int bcd)
{
    if(gameInfo.getIsLTARGame())    return bcd;     //LTAR hosting protocol does not use BCD

    if (bcd == 0xFF) return bcd;
    return (int) (((bcd >> 4) & 0xF) *10) + (bcd & 0xF);
}

void LttoComms::nonBlockingDelay(int mSec)
{
    QEventLoop loop;
    QTimer::singleShot(mSec, &loop, SLOT(quit()));
    loop.exec();
}
