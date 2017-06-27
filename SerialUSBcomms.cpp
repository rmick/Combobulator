#include "SerialUSBcomms.h"
#include "ui_SerialUSBcomms.h"
#include <QtSerialPort>
#include "LttoComms.h"

SerialUSBcomms serialUSBcomms;

SerialUSBcomms::SerialUSBcomms(QObject *parent) :
    QObject(parent)
    //ui(new Ui::SerialUSBcomms)
{
    //ui->setupUi(this);
    serialUSB = new QSerialPort(this);

    connect(serialUSB,  SIGNAL(readyRead()),                    this, SLOT(receivePacket()) );
    connect(&lttoComms, SIGNAL(sendSerialData(QByteArray)),     this, SLOT(sendPacket(QByteArray)));
    connect(this,       SIGNAL(newSerialUSBdata(QByteArray)),   &lttoComms, SLOT(receivePacket(QByteArray)) );
}

//SerialUSBcomms::~SerialUSBcomms()
//{
//    delete ui;
//}

void SerialUSBcomms::findSerialPort()
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
        qDebug() << "SerialPort is Ready..." << endl;
    }
    else
        qDebug() << "SerialPort failed to open..." << endl;
}

void SerialUSBcomms::closeSerialPort()
{
    serialUSB->close();
    qDebug() << "SerialPort has left the building";
}

void SerialUSBcomms::receivePacket()
{
    QByteArray rX;
    rX = serialUSB->readAll();
    qDebug() << "SerialUSBcomms::receivePacket " << rX;
    emit newSerialUSBdata(rX);
}

void SerialUSBcomms::sendPacket(QByteArray packet)
{
    bool result = false;
    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        serialUSB->write(packet);
        serialUSB->flush();
        result = true;
    }
    else qDebug() << "SerialUSBcomms::sendPacket() - FAILED !!!";
}
