#include "SerialUSBcomms.h"
#include "LttoComms.h"

SerialUSBcomms serialUSBcomms;

SerialUSBcomms::SerialUSBcomms(QObject *parent) :
    QObject(parent)
{
#ifdef INCLUDE_SERIAL_USB
    serialUSB = new QSerialPort(this);
    connect(serialUSB,  SIGNAL(readyRead()),                    this,       SLOT(receivePacket()) );
#endif
    connect(&lttoComms, SIGNAL(sendSerialData(QByteArray)),     this,       SLOT(sendPacket(QByteArray)) );
    connect(this,       SIGNAL(newSerialUSBdata(QByteArray)),   &lttoComms, SLOT(receivePacket(QByteArray)) );
}

void SerialUSBcomms::findSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.manufacturer().contains("Spark") || info.manufacturer().contains("Arduino") || info.portName().contains("tty") )
        {
            serialUSB->setPortName(info.portName() );
            emit SerialPortFound(info.portName() );
            //qDebug() << "SerialUSBcomms::findSerialPort() - Using serial port " << info.portName();
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
    }
    else
        qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " failed to open..." << endl;
#endif
}

void SerialUSBcomms::closeSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    serialUSB->close();
    qDebug() << "SerialUSBcomms::closeSerialPort() has left the building";
#endif
}

void SerialUSBcomms::receivePacket()
{
#ifdef INCLUDE_SERIAL_USB
    QByteArray rX;
    rX = serialUSB->readAll();
    //qDebug() << "SerialUSBcomms::receivePacket() " << rX;
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
#endif
}


