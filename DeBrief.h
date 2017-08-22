#ifndef DEBRIEF_H
#define DEBRIEF_H

#include <QObject>
#include <QTimer>

class DeBrief : public QObject
{
    Q_OBJECT
public:
    explicit DeBrief(QObject *parent = nullptr);

signals:

public slots:
    void RequestTagReports(int playerToInterogate);


};

#endif // DEBRIEF_H
