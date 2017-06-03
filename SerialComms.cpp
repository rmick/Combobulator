#include "SerialComms.h"
#include <QtSerialPort>
#include "HostGameWindow.h"

SerialComms serialComms;

SerialComms::SerialComms(QObject *parent) : QObject(parent)
{

    serialUSB = new QSerialPort(this);
    QObject::connect(serialUSB, SIGNAL(readyRead()), this, SLOT(receivePacket()) );

    useLazerSwarm = true;          //TODO: Set this up in Preferences.

}

void SerialComms::findSerialPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.manufacturer().contains("Spark") || info.manufacturer().contains("Arduino") || info.portName().contains("tty") )
        {
            serialUSB->setPortName(info.portName() );
            emit SerialPortFound(info.portName() );
        }
        qDebug() << "\n-----------\nPortToUse=" << info.portName() << info.manufacturer() << info.description();
    }
}

void SerialComms::setUpSerialPort()
{

    findSerialPort();
        #ifdef Q_OS_ANDROID
        serialUSB->setPortName("ttyS0");
        #endif
    serialUSB->open(QIODevice::ReadWrite);
    serialUSB->setBaudRate(QSerialPort::Baud115200);
    serialUSB->setDataBits(QSerialPort::Data8);
    serialUSB->setParity(QSerialPort::NoParity);
    serialUSB->setStopBits(QSerialPort::OneStop);
    serialUSB->setFlowControl(QSerialPort::NoFlowControl);

    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        qDebug() << "SerialPort is Ready..." << endl;
    }
    else
        qDebug() << "SerialPort failed to open..." << endl;
}

void SerialComms::closeSerialPort()
{
    serialUSB->close();
    qDebug() << "SerialPort has left the building";
}

int SerialComms::getRxPacket()
{
    return rxPacket.command;
}

////////////////////////////////////////////////////////////////////

bool SerialComms::sendPacket(char type, QString data)
{
    bool result = false;
    QByteArray packet;
    bool ok; // Dummy

    //Calculating the CheckSum.
    //This is here because it always does my head in !!!!!
    //The Data is BCD Hex (whatever that means).        It means it is 2 x 4bit numbers in a single Byte
    //                                                  Therefore 10 is actually 0001 0000 (which is 0x16)
    //                                                  and       42 is actually 0100 0010 (which is 0x64)
    //                                                  and       79 is actually 0111 1001 (which is 0x121)
    //                                                  and      255 is actually 1111 1111 (which is 0xFF)
    //This means that '10' is actually 0x16,
    //and the CheckSum is the Sum of the Hex values, not the BCD.
    //This is confusing as 10 minutes in GameTime is sent as '10'
    //but is actually 0x16 as far as the CheckSum is concerned.

    switch (type)
    {
        case PACKET:
            calculatedCheckSumTx = data.toInt(&ok, 16);
            packet.append("P" + data);
            break;
        case DATA:
            calculatedCheckSumTx += data.toInt(&ok, 16);
            packet.append("D" + data);
            break;
        case CHECKSUM:
            calculatedCheckSumTx = calculatedCheckSumTx % 256;      // CheckSum is the remainder of dividing by 256.
            calculatedCheckSumTx = calculatedCheckSumTx | 256;      // Set the required 9th MSB bit to 1 to indicate it is a checksum
            packet.append("C" + QString::number(calculatedCheckSumTx, 16).toUpper() );
            break;
        case TAG:
            packet.append("T" + data);
            break;
        case BEACON:
            packet.append("B" + data);
            break;
    }

    if (useLazerSwarm)
    {
        QByteArray packetToTranslate;
        packetToTranslate.append(packet);
        packet.clear();
        packet.append(lazerswarm.translateCommand(packetToTranslate) );
        packet.append(" \r\n");
        qDebug() << "  packetSent->" << packetToTranslate << "\t" << packet;
    }

    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        //qDebug() << "  packetSent->" << packet;
        serialUSB->write(packet);
        serialUSB->flush();
        //emit sendSerialData(packet);
        result = true;
        QThread::msleep(80);

    }
    return result;
}

////////////////////////////////////////////////////////////////////

void SerialComms::receivePacket()
{
    //emit TimerBlock(true);
    bool processed = false;

    if (useLazerSwarm)
    {
        irDataIn.append(serialUSB->readAll() );

        if (irDataIn.endsWith("\n") == false)
        {
            irDataIn.append(serialUSB->readAll() );
        }
        else
        {
            rxPacketList.append(lazerswarm.decodeCommand(irDataIn));
            qDebug() << "SerialComms::receivePacket() " << lazerswarm.decodeCommand(irDataIn);
            processed = true;
        }
    }
    else
    {
        irDataIn.append(serialUSB->readAll() );
        //TODO: Check for full packet/s (eg. "C86")
        rxPacketList.append(irDataIn);
        qDebug() << "SerialComms::receivePacket() - This code is not yet written";
        processed = true;
    }

    if (processed)
    {
        irDataIn.clear();
        if (!rxPacketList.empty())
        {
            if (rxPacketList.last().startsWith("C1") )
            {
                processPacket(rxPacketList);
            }
            //emit TimerBlock(false);
        }
    }
}

bool SerialComms::isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum)
{
    bool result = false;
    calculatedCheckSumRx =  _command;
    calculatedCheckSumRx += _game;
    calculatedCheckSumRx += _tagger;
    calculatedCheckSumRx += _flags;
    calculatedCheckSumRx = calculatedCheckSumRx%256;
    if (calculatedCheckSumRx ==  (_checksum%256) ) result = true;
    qDebug() << "   SerialComms::isCheckSumCorrect()" << calculatedCheckSumRx << ":" << _checksum%256 << "Result = " << result;
    return result;
}


void SerialComms::processPacket(QList<QByteArray> data)
{

    int command = extract(data);
    int game    = 0;
    int tagger  = 0;
    int flags   = 0;
    int checksum= 0;

    qDebug() << "SerialComms::processPacket()" << command;

    switch (command)
    {
        case 0x10:
            game     = extract(data);
            tagger   = extract(data);
            flags    = extract(data);
            checksum = extract(data);
            if(isCheckSumCorrect(command, game, tagger, flags, checksum) == false) break;
            emit RequestJoinGame(game, tagger, flags);
            qDebug() << "emit RequestJoinGame()" << game << tagger << flags << checksum;
            break;
        case 0x11:
            game     = extract(data);
            tagger   = extract(data);
            checksum = extract(data);
            if(isCheckSumCorrect(command, game, tagger, flags, checksum) == false) break;
            emit AckPlayerAssignment(game, tagger);
            qDebug() << "emit AckPlayerAssignment()" << game << tagger << checksum;
            break;
        //Other cases will be required for DeBrief. Maybe create a funciton for each one to make code more easily readable.

    }
    rxPacketList.clear();
}


int SerialComms::extract(QList<QByteArray> &data)
{
    QString command = data.takeFirst();
    command = command.mid(1 , (command.length()-1) );
    bool ok = false;
    return command.toInt(&ok, 16);
}


int SerialComms::ConvertDecToBCD(int dec)
{
  if (dec == 0xFF) return dec;
  return (int) (((dec/10) << 4) | (dec %10) );
}

int SerialComms::ConvertBCDtoDec(int bcd)
{
  if (bcd == 0xFF) return bcd;
  return (int) (((bcd >> 4) & 0xF) *10) + (bcd & 0xF);
}



