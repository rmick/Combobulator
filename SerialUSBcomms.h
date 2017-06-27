#ifndef SERIALUSBCOMMS_H
#define SERIALUSBCOMMS_H

#include <QObject>
#include <QtSerialPort/QSerialPortInfo>
#include "Defines.h"

class SerialUSBcomms : public QObject
{
    Q_OBJECT
#ifdef  INCLUDE_SERIAL_USB
    public:
        explicit SerialUSBcomms(QObject *parent = 0);
        //~SerialUSBcomms();

        void        findSerialPort();
        void        setUpSerialPort();
        void        closeSerialPort();

    signals:
        void        SerialPortFound(QString portDetails);
        //void        sendSerialData(QByteArray dataToSend);
        void        newSerialUSBdata(QByteArray dataReceived);

public slots:
        void        receivePacket();
        void        sendPacket(QByteArray packet);

    private:
            QSerialPort     *serialUSB;
};

extern SerialUSBcomms serialUSBcomms;

#endif // INCLUDE_SERIAL_USB

#endif // SERIALUSBCOMMS_H
