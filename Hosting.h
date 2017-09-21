#ifndef HOSTING_H
#define HOSTING_H

#include <QObject>
#include "Game.h"
#include "Players.h"

class Hosting : public QObject
{
    Q_OBJECT
public:
    explicit Hosting(QObject *parent = nullptr);
    bool pickTheKing();
    bool assignSpies();
    int  GetRandomNumber(int min, int max);

signals:
    void    AddToHostWindowListWidget(QString rhubarbRhubarb);

public slots:

private:

};

extern Hosting host;

#endif // HOSTING_H
