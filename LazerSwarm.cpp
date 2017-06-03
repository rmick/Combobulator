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
        case 'P':
            numberOfBits = " 9";       //last bit must be 0
            isBeacon     = " 0";
        break;

        case 'D':
            numberOfBits = " 8";
            isBeacon     = " 0";
        break;

        case 'C':
            numberOfBits = " 9";       //last bit must be 1
            isBeacon     = " 0";
        break;

        case 'T':
            numberOfBits = " 7";
            isBeacon     = " 0";
        break;

        case 'B':
            numberOfBits = " 5";
            isBeacon     = " 1";
        break;
    }
    command = messageOut.remove(0,1);
    //qDebug() << "LZ trans= " << command << numberOfBits << isBeacon;
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
        int numberOfBitsIn;
        int isBeaconIn;

        messageIn.trimmed();                                                //removes the \r\n from the end
        messageParts = messageIn.split(" ");

        bool ok = 0;
        if(!messageParts.empty()) messageParts.removeFirst();                 //Discard the RCV

        if(!messageParts.empty()) packetDataIn   = messageParts.takeFirst().toInt(&ok, 16); else packetDataIn   = -1;
        if (!ok) qDebug() << "LazerSwarm::decodeCommand() - error toInt(16)";
        if(!messageParts.empty()) numberOfBitsIn = messageParts.takeFirst().toInt(&ok, 16); else numberOfBitsIn = -1;
        if (!ok) qDebug() << "LazerSwarm::decodeCommand() - error toInt(16)";
        if(!messageParts.empty()) isBeaconIn     = messageParts.takeFirst().toInt(&ok, 16); else isBeaconIn     = -1;
        if (!ok) qDebug() << "LazerSwarm::decodeCommand() - error toInt(16)";
        if(!messageParts.empty()) qDebug() << "SPARE DATA !!!!  LazerSwarm::decodeCommand()" << messageParts.takeFirst();

        //qDebug() << "LazerSwarm::decodeCommand(QString messageIn)" << messageIn << ":" << packetDataIn << ":" << numberOfBitsIn << ":" << isBeaconIn;

        //assemble message
        if      (numberOfBitsIn == 9 && packetDataIn < 256)     packetTypeIn = 'P';
        else if (numberOfBitsIn == 9 && packetDataIn > 256)     packetTypeIn = 'C';
        else if (numberOfBitsIn == 8 && !isBeaconIn)            packetTypeIn = 'D';
        else if (numberOfBitsIn == 5 &&  isBeaconIn)            packetTypeIn = 'B';
        else if (numberOfBitsIn == 7 && !isBeaconIn)            packetTypeIn = 'T';
        else                                                    packetTypeIn = '_';

        decodedMessage.append(packetTypeIn);
        decodedMessage.append(QString::number(packetDataIn, 16).toUpper() );
    }
    else decodedMessage = "xxx";

    return decodedMessage;
}
