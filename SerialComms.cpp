

#include "SerialComms.h"
#include <QtSerialPort>
#include "HostGameWindow.h"

SerialComms serialComms;

SerialComms::SerialComms(QObject *parent) : QObject(parent)
{

    serialUSB = new QSerialPort(this);
    //delayTimer      = new QTimer(this);

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

        //qDebug() << "\n-----------\nPortToUse=" << info.portName() << info.manufacturer() << info.description();
    }
}

void SerialComms::setUpSerialPort()
{

    findSerialPort();
        #ifdef Q_OS_ANDROID
        serialUSB->setPortName("ttyS1");
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

bool SerialComms::sendPacket(char type, int data, bool dataFormat)
{
    bool result = false;
    //bool ok;    //dummy
    QByteArray packet;

    //Calculating BCD and the CheckSum.
    //This is here because it always does my head in !!!!!
    //Certain Data is BCD Hex (whatever that means).        It means it is 2 x 4bit numbers in a single Byte
    //                                                      Therefore 10 is actually 0001 0000 (which is 0x16)
    //                                                      and       42 is actually 0100 0010 (which is 0x64)
    //                                                      and       79 is actually 0111 1001 (which is 0x121)
    //                                                      and      255 is actually 1111 1111 (which is 0xFF)
    //This means that '10' is actually 0x16.
    //However other data (taggerID, GameID, Flags, etc) are just plain Hex/Dec all the time.
    //The LazerSwarm then needs aal data sent as actual Hex, whereas the LTTO library expects the Dec '10'.
    //The CheckSum is the Sum of the final Hex values or Decimal values AFTER conversion (where applicable) to BCD.

    switch (type)
    {
        case PACKET:
            calculatedCheckSumTx = data;
            packetString = createIRstring(data);
            packetString.prepend('P');
            packet.append(packetString);
            //qDebug() << "SerialComms::sendPacket() - Packet =    " << data << "\t:" << packetString;
            break;
        case DATA:
            if (dataFormat == BCD)
            {
                calculatedCheckSumTx += ConvertDecToBCD(data);
                data = ConvertDecToBCD(data);
                packetString = createIRstring(data);
                packetString.prepend('D');
                packet.append(packetString);
                //qDebug() << "SerialComms::sendPacket() - BCD Data = " << data << "\t:" << packetString << "\tBCD=" << dataFormat;
            }
            else
            {
                calculatedCheckSumTx += data;
                packetString = createIRstring(data);
                packetString.prepend('D');
                packet.append(packetString);
                //qDebug() << "SerialComms::sendPacket() - Dec Data = " << data << "\t:" << packetString << "\tBCD=" << dataFormat;
            }
            break;
        case CHECKSUM:
            calculatedCheckSumTx = calculatedCheckSumTx % 256;      // CheckSum is the remainder of dividing by 256.
            calculatedCheckSumTx = calculatedCheckSumTx | 256;      // Set the required 9th MSB bit to 1 to indicate it is a checksum
            packetString = createIRstring(calculatedCheckSumTx);
            packetString.prepend('C');
            packet.append(packetString);
            //qDebug() << "SerialComms::sendPacket() - CheckSum = \t" << packetString;
            break;
        case TAG:
            packetString = createIRstring(data);
            packetString.prepend('T');
            packet.append(packetString);
            break;
        case BEACON:
            packetString = createIRstring(data);
            packetString.prepend('B');
            packet.append(packetString);
            break;
    }

    if (useLazerSwarm)
    {
        QByteArray packetToTranslate;
        packetToTranslate.append(packet);
        packet.clear();
        packet.append(lazerswarm.translateCommand(packetToTranslate) );
        packet.append(" \r\n");
    }
    else packet.append(":");

    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        serialUSB->write(packet);
        //serialUSB->flush();
        result = true;
    }
    qDebug() << "SerialComms::sendPacket  USBpacketSent->" << packet;
    emit sendSerialData(packet);            //Connects to TCPComms::sendData slot && USBComms::sendData slot
    //QThread::msleep(INTERPACKET_DELAY_MSEC);
    blockingDelay(INTERPACKET_DELAY_MSEC);

    return result;
}

QString SerialComms::createIRstring(int data)
{
    QString createdPacket;
    if (useLazerSwarm) createdPacket = QString::number(data,16).toUpper();
    else               createdPacket = QString::number(data,10);
    if (createdPacket.length() == 1) createdPacket.prepend('0');
    //qDebug() << "SerialComms::createIRstring()" << createdPacket;
    return createdPacket;
}

////////////////////////////////////////////////////////////////////

void SerialComms::receivePacket()
{
    emit TimerBlock(true);
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
            qDebug() << "SerialComms::receivePacket() LazerSwarm mode - " << lazerswarm.decodeCommand(irDataIn);
            processed = true;
        }
    }
    else
    {
        irDataIn.append(serialUSB->readAll() );
        if (irDataIn.endsWith(":") == false)
        {
            irDataIn.append(serialUSB->readAll() );
        }
        rxPacketList.append(irDataIn);
        qDebug() << "SerialComms::receivePacket() LTTO library mode - " << irDataIn;
        processed = true;
    }



    if (processed)
    {
        irDataIn.clear();
        if (!rxPacketList.empty())
        {
            if (rxPacketList.last().startsWith("C") )
            {
                processPacket(rxPacketList);
            }
            emit TimerBlock(false);
        }
    }
}

bool SerialComms::getUseLazerSwarm() const
{
    return useLazerSwarm;
}

void SerialComms::setUseLazerSwarm(bool value)
{
    useLazerSwarm = value;
}

bool SerialComms::isCheckSumCorrect(int _command, int _game, int _tagger, int _flags, int _checksum)
{
    bool result = false;
    calculatedCheckSumRx =  _command;
    calculatedCheckSumRx += _game;
    calculatedCheckSumRx += _tagger;
    calculatedCheckSumRx += _flags;
    calculatedCheckSumRx = calculatedCheckSumRx%256;
    if (calculatedCheckSumRx == _checksum%256) result = true;
    return result;
}


void SerialComms::processPacket(QList<QByteArray> data)
{

    int command = extract(data);
    int game    = 0;
    int tagger  = 0;
    int flags   = 0;
    int checksum= 0;

    qDebug() << "\nSerialComms::processPacket()" << command;

    switch (command)
    {
        case REQUEST_JOIN_GAME:
            game     = extract(data);
            tagger   = extract(data);
            flags    = extract(data);
            checksum = extract(data);
            if(isCheckSumCorrect(command, game, tagger, flags, checksum) == false) break;

            emit RequestJoinGame(game, tagger, flags);
            //qDebug() << "emit RequestJoinGame()" << game << tagger << flags << checksum;
            break;

        case ACK_PLAYER_ASSIGN:
            game     = extract(data);
            tagger   = extract(data);
            checksum = extract(data);
            if(isCheckSumCorrect(command, game, tagger, flags, checksum) == false) break;

            emit AckPlayerAssignment(game, tagger);
            //qDebug() << "emit AckPlayerAssignment()" << game << tagger << checksum;
            break;

        //Other cases will be required for DeBrief. Maybe create a funciton for each one to make code more easily readable.

    }
    rxPacketList.clear();
}


int SerialComms::extract(QList<QByteArray> &data)
{
    if (data.empty() == false)
    {
        QString command = data.takeFirst();
        command = command.mid(1 , (command.length()-1) );
        bool ok = false;
        return command.toInt(&ok, 10);
    }
}

int SerialComms::ConvertDecToBCD(int dec)
{
  if (dec == 100) return 0xFF;
  return (int) (((dec/10) << 4) | (dec %10) );
}

int SerialComms::ConvertBCDtoDec(int bcd)
{
  if (bcd == 0xFF) return bcd;
  return (int) (((bcd >> 4) & 0xF) *10) + (bcd & 0xF);
}

void SerialComms::blockingDelay(int mSec)
{
    QEventLoop loop;
    QTimer::singleShot(mSec, &loop, SLOT(quit()));
    loop.exec();
}
