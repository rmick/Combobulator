#ifndef SERIALUSBCOMMS_H
#define SERIALUSBCOMMS_H


#include <QObject>
#include "Defines.h"

#ifdef  INCLUDE_SERIAL_USB
    #include <QtSerialPort/QSerialPortInfo>
    #include <QtSerialPort>
#endif

class SerialUSBcomms : public QObject
{
    Q_OBJECT

public:
    explicit SerialUSBcomms(QObject *parent = 0);

    void        findSerialPort();
    void        setUpSerialPort();
    void        closeSerialPort();

signals:
    void        SerialPortFound(QString portDetails);
    void        newSerialUSBdata(QByteArray dataReceived);
    void        AddToHostWindowListWidget(QString rhubarbRhubarb);

public slots:
    void        receivePacket();
    void        sendPacket(QByteArray packet);

private:
#ifdef INCLUDE_SERIAL_USB
        QSerialPort     *serialUSB;
#endif
};

extern SerialUSBcomms serialUSBcomms;

#endif // SERIALUSBCOMMS_H
