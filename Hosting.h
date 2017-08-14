#ifndef HOSTING_H
#define HOSTING_H

#include <QObject>

class Hosting : public QObject
{
    Q_OBJECT
public:
    explicit Hosting(QObject *parent = nullptr);

signals:

public slots:
};

#endif // HOSTING_H