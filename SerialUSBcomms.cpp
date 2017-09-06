#include "SerialUSBcomms.h"
#include "LttoComms.h"

SerialUSBcomms serialUSBcomms;

SerialUSBcomms::SerialUSBcomms(QObject *parent) :
    QObject(parent)
{
    qDebug() << "SerialUSBComms::SerialUSBComms() - Constructing.......";
#ifdef INCLUDE_SERIAL_USB
    serialUSB = new QSerialPort(this);
    connect(serialUSB,  SIGNAL(readyRead()),                    this,       SLOT(receivePacket()) );
#endif
    setSerialCommsConnected(false);
    setIsUSBinitialised(false);
}

void SerialUSBcomms::findSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.manufacturer().contains("Spark") || info.manufacturer().contains("Arduino") )  // || info.portName().contains("SLAB") )
        {
            serialUSB->setPortName(info.portName() );
            emit SerialPortFound(info.portName() );
        }
        QString Rhubarb = "SerialUSBcomms::findSerialPort() - Using serial port " + info.portName();
        emit AddToHostWindowListWidget(Rhubarb);
    }
#endif
}

void SerialUSBcomms::setUpSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    findSerialPort();
        #ifdef Q_OS_ANDROID
        return;
        #endif

        serialUSB->open(QIODevice::ReadWrite);
        serialUSB->setBaudRate(QSerialPort::Baud115200);
        serialUSB->setDataBits(QSerialPort::Data8);
        serialUSB->setParity(QSerialPort::NoParity);
        serialUSB->setStopBits(QSerialPort::OneStop);
        serialUSB->setFlowControl(QSerialPort::NoFlowControl);

        if (serialUSB->isOpen() && serialUSB->isWritable())
        {
            qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " is Ready..." << endl;
            setSerialCommsConnected(true);
        }
        else
        {
            qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " failed to open..." << endl;
            setSerialCommsConnected(false);
        }
#endif
}

void SerialUSBcomms::closeSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    if(serialCommsConnected)
    {
        //serialUSB->close();
        setSerialCommsConnected(false);
        qDebug() << "SerialUSBcomms::closeSerialPort() has left the building";
    }
#endif
}

void SerialUSBcomms::receivePacket()
{
#ifdef INCLUDE_SERIAL_USB
    QByteArray rX;
    rX = serialUSB->readAll();
    //qDebug() << "SerialUSBcomms::receivePacket() " << rX;
#ifdef Q_OS_ANDROID
    lttoComms.androidRxPacket(rX);
#endif

    emit newSerialUSBdata(rX);
#endif
}

void SerialUSBcomms::sendPacket(QByteArray packet)
{
#ifdef INCLUDE_SERIAL_USB
    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        serialUSB->write(packet);
        serialUSB->flush();
        //qDebug() << "SerialUSBcomms::sendPacket() - " << packet;
    }
    //else qDebug() << "SerialUSBcomms::sendPacket() - FAILED !!!";
#else
    packet.clear();        // to silence Compiler warning!
#endif
}

bool SerialUSBcomms::getIsUSBinitialised() const
{
    return isUSBinitialised;
}

void SerialUSBcomms::setIsUSBinitialised(bool value)
{
    isUSBinitialised = value;
}

bool SerialUSBcomms::getSerialCommsConnected() const
{
    return serialCommsConnected;
}

void SerialUSBcomms::setSerialCommsConnected(bool value)
{
    serialCommsConnected = value;
}

void SerialUSBcomms::initialiseUSBsignalsAndSlots()
{
    //Needs to be done here not in constructor because lttoComms a static and may not exist when this static is created.
    qDebug() << "SerialUSBcomms::initialiseUSBsignalsAndSlots() - Triggered";
    connect(&lttoComms, SIGNAL(sendSerialData(QByteArray)),     this,       SLOT(sendPacket(QByteArray)) );
    connect(this,       SIGNAL(newSerialUSBdata(QByteArray)),   &lttoComms, SLOT(receivePacket(QByteArray)) );
    connect(serialUSB,  SIGNAL(readyRead()),                    this,       SLOT(receivePacket()) );
    setIsUSBinitialised(true);
}


