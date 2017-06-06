#ifndef LAZERSWARM_H
#define LAZERSWARM_H

#include "QString"
#include "Defines.h"

class LazerSwarm
{
public:
    LazerSwarm();

    QString     translateCommand(QString messageOut);
    QByteArray  decodeCommand(QString messageIn);

private:
    QString     command;            //The command to send (sent as Hex)
    QString     numberOfBits;       //The number of bits to send (9/8)
    QString     isBeacon;           //true if the message is a Beacon
};

extern LazerSwarm lazerswarm;

#endif // LAZERSWARM_H
