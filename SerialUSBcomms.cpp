#include "SerialUSBcomms.h"
#include "LttoComms.h"
//#include "LttoMainWindow.h"

SerialUSBcomms serialUSBcomms;

SerialUSBcomms::SerialUSBcomms(QObject *parent) :
    QObject(parent)
{
    serialUSB = new QSerialPort(this);
    connect(serialUSB,  SIGNAL(readyRead()),                    this,       SLOT(receivePacket()) );

    connect(&lttoComms, SIGNAL(sendSerialData(QByteArray)),     this,       SLOT(sendPacket(QByteArray)) );
    connect(this,       SIGNAL(newSerialUSBdata(QByteArray)),   &lttoComms, SLOT(receivePacket(QByteArray)) );

    //connect(this,       SIGNAL(AddToHostWindowListWidget(QString)), &hostGameWindow, SLOT(InsertToListWidget(QString)) );
}

void SerialUSBcomms::findSerialPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (info.manufacturer().contains("Spark") || info.manufacturer().contains("Arduino") ) // || info.portName().contains("tty") )
        {
            serialUSB->setPortName(info.portName() );
            emit SerialPortFound(info.portName() );
            qDebug() << "SerialUSBcomms::findSerialPort() - Using serial port " << info.portName();
        }
        QString Rhubarb = "SerialUSBcomms::findSerialPort() - Using serial port " + info.portName();
        emit AddToHostWindowListWidget(Rhubarb);
    }
}

void SerialUSBcomms::setUpSerialPort()
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
        qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " is Ready..." << endl;
    }
    else
        qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " failed to open..." << endl;
}

void SerialUSBcomms::closeSerialPort()
{
    serialUSB->close();
    qDebug() << "erialUSBcomms::closeSerialPort() has left the building";
}

void SerialUSBcomms::receivePacket()
{
    QByteArray rX;
    rX = serialUSB->readAll();
    qDebug() << "SerialUSBcomms::receivePacket() " << rX;
    emit newSerialUSBdata(rX);
}

void SerialUSBcomms::sendPacket(QByteArray packet)
{
    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        serialUSB->write(packet);
        serialUSB->flush();
        qDebug() << "SerialUSBcomms::sendPacket() - " << packet;
    }
    //else qDebug() << "SerialUSBcomms::sendPacket() - FAILED !!!";
}
