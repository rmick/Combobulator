#include "LazerSwarm.h"
#include <QDebug>
#include <QList>

LazerSwarm lazerswarm;

LazerSwarm::LazerSwarm()
{
    command         = " Q";
    numberOfBits    = " 0";
    isBeacon        = " 0";
}

QString LazerSwarm::translateCommand(QString messageOut)
{
    //convert message (eg. D02) into command/numberOfBits/isBeacon)
    char packetType = QString(messageOut)[0].unicode();
    switch(char(packetType))
    {
        case PACKET:
            numberOfBits = " 9";       //last bit must be 0
            isBeacon     = " 0";
        break;

        case DATA:
            numberOfBits = " 8";
            isBeacon     = " 0";
        break;

        case CHECKSUM:
            numberOfBits = " 9";       //last bit must be 1
            isBeacon     = " 0";
        break;

        case TAG:
            numberOfBits = " 7";
            isBeacon     = " 0";
        break;

        case BEACON:
            numberOfBits = " 5";
            isBeacon     = " 1";
        break;
    }
    command = messageOut.remove(0,1);   //Remove the char packetType
    //translate command from DEC to HEX
    QString translated = "CMD 10 ";
    translated.append(command);
    translated.append(numberOfBits);
    translated.append(isBeacon);
    return translated;
}

QByteArray LazerSwarm::decodeCommand(QString messageIn)
{
    QList<QString>  messageParts;
    QByteArray      decodedMessage;

    if (messageIn.startsWith("RCV") )
    {
        char packetTypeIn;
        int  packetDataIn;
        int  numberOfBitsIn;
        int  isBeaconIn;

        messageIn.trimmed();                                                //removes the \r\n from the end
        messageParts = messageIn.split(" ");

        bool ok = 0;
        if(!messageParts.empty()) messageParts.removeFirst();               //Discard the RCV
        if(!messageParts.empty()) packetDataIn   = messageParts.takeFirst().toInt(&ok, 16); else packetDataIn   = -1;
        if(!messageParts.empty()) numberOfBitsIn = messageParts.takeFirst().toInt(&ok, 16); else numberOfBitsIn = -1;
        if(!messageParts.empty()) isBeaconIn     = messageParts.takeFirst().toInt(&ok, 16); else isBeaconIn     = -1;

        //qDebug() << "LazerSwarm::decodeCommand()" << messageIn << "--" << packetDataIn << ":" << numberOfBitsIn << ":" << isBeaconIn;

        //assemble message
        if      (numberOfBitsIn == 9 && packetDataIn < 256)     packetTypeIn = PACKET;
        else if (numberOfBitsIn == 9 && packetDataIn > 256)     packetTypeIn = CHECKSUM;
        else if (numberOfBitsIn == 8 && !isBeaconIn)            packetTypeIn = DATA;
        else if (numberOfBitsIn == 5 &&  isBeaconIn)            packetTypeIn = BEACON;
        else if (numberOfBitsIn == 7 && !isBeaconIn)            packetTypeIn = TAG;
        else                                                    packetTypeIn = '_';     // Indicates an error.

        decodedMessage.append(packetTypeIn);
        decodedMessage.append(QString::number(packetDataIn, 10).toUpper() );
        //qDebug() << "LazerSwarm::decodeCommand() - Decoded Message = " << decodedMessage;
    }
    else decodedMessage = "xxx";

    return decodedMessage;
}
