#ifndef DEBRIEF_H
#define DEBRIEF_H

#include <QObject>

class DeBrief : public QObject
{
    Q_OBJECT
public:
    explicit DeBrief(QObject *parent = nullptr);

signals:

public slots:
    void RequestTagReports();
};

#endif // DEBRIEF_H
