#include "SerialUSBcomms.h"
#include <QDebug>

SerialUSBcomms serialUSBcomms;

SerialUSBcomms::SerialUSBcomms(QObject *parent) :
    QObject(parent)
{
    qDebug() << "SerialUSBComms::SerialUSBComms() - Constructing.......";
#ifdef INCLUDE_SERIAL_USB
    serialUSB = new QSerialPort(this);
#endif
    setSerialCommsConnected(false);
    setIsUSBinitialised(false);
}

void SerialUSBcomms::findSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //qDebug() << "SerialUSBcomms::findSerialPort() - Found a port:" << info.portName();
		if (info.manufacturer().contains("Spark") || info.manufacturer().contains("Arduino")
				|| info.portName().contains("usbmodem") || info.portName().contains("COM")
				|| info.description().contains("freetronics") )
        {
            serialUSB->setPortName(info.portName() );
            emit SerialPortFound(info.portName() );
			QString Rhubarb = "SerialUSBcomms::findSerialPort() - Using serial port " + info.portName();
			qDebug() << "SerialUSBcomms::findSerialPort() - Using:" << info.portName();
			emit AddToHostWindowListWidget(Rhubarb);
		}
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

        if (serialUSB->isReadable()) qDebug() << "SerialUSBcomms::setUpSerialPort() -" << serialUSB->portName() << "is READABLE :-)";
        if (serialUSB->isOpen() && serialUSB->isWritable())
        {
        qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " is Ready..." << Qt::endl;
            setSerialCommsConnected(true);
        }
        else
        {
            //qDebug() << "SerialUSBcomms::setUpSerialPort()" << serialUSB->portName() << " failed to open..." << endl;
            setSerialCommsConnected(false);
        }
#endif
}

void SerialUSBcomms::closeSerialPort()
{
#ifdef INCLUDE_SERIAL_USB
    if(serialCommsConnected)
    {
        serialUSB->close();
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
    #ifdef Q_OS_ANDROID
	//lttoComms.androidRxPacket(rX);
	//TODO: Is this fixed?
	emit newSerialUSBdata(rX);
    #else
    emit newSerialUSBdata(rX);
    #endif
#endif
}

//void SerialUSBcomms::readUSBdata()
//{
//     QByteArray rX;
//     rX = serialUSB->readAll();
//     qDebug() << "SerialUSBcomms::readUSBdata() - Does this ever get called???????" << rX;
//}

void SerialUSBcomms::sendPacket(QByteArray packet)
{
#ifdef INCLUDE_SERIAL_USB
    if (serialUSB->isOpen() && serialUSB->isWritable())
    {
        //qDebug() << "\tSending serial to USB";
        serialUSB->write(packet);
        serialUSB->flush();
    }
	else
	{
        serialUSBcomms.setUpSerialPort();
	}
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
//TODO1: Hmmmmmmmm			lttoComms.setSerialUSBcommsConnected(value);
}

void SerialUSBcomms::initialiseUSBsignalsAndSlots()
{
    //Needs to be done here not in constructor because lttoComms is a static and may not exist when this static is created.
#ifdef INCLUDE_SERIAL_USB
	qDebug() << "SerialUSBcomms::initialiseUSBsignalsAndSlots() - Triggered";
    connect(serialUSB,  SIGNAL(readyRead()),                    this,       SLOT(receivePacket()) );
    setIsUSBinitialised(true);
#endif
}


