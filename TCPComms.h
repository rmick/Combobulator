#ifndef TCPCOMMS_H
#define TCPCOMMS_H

#include <QDialog>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>

namespace Ui {
class TCPComms;
}

class TCPComms : public QDialog
{
    Q_OBJECT

public:
    explicit TCPComms(QWidget *parent = 0);
    ~TCPComms();
    //void Test();

public slots:
    void connected();
    void disconnected();
    void bytesWritten (qint64 bytes);
    void readyRead();
    void sendData(QByteArray data);

private slots:
    void on_btn_Connect_clicked();

private:
    Ui::TCPComms    *ui;
    QTcpSocket      *socket;
};

#endif // TCPCOMMS_H
