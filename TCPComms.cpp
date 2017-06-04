#include "TCPComms.h"
#include "ui_TCPComms.h"

#include "SerialComms.h"

TCPComms::TCPComms(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TCPComms)
{
    ui->setupUi(this);

    socket = new QTcpSocket(this);

    connect(socket,         SIGNAL(connected()),                this, SLOT(connected()));
    connect(socket,         SIGNAL(disconnected()),             this, SLOT(disconnected()));
    connect(socket,         SIGNAL(readyRead()),                this, SLOT(readyRead()));
    connect(socket,         SIGNAL(bytesWritten(qint64)),       this,SLOT(bytesWritten(qint64)));
    connect(&serialComms,   SIGNAL(sendSerialData(QByteArray)), this, SLOT(sendData(QByteArray)));

    qDebug() << "Connecting...";

    socket->connectToHost("192.168.2.1",8000);
    ui->listWidget_TCP->addItem("Connecting to LTTO_host");
}

TCPComms::~TCPComms()
{
    delete ui;
}

void TCPComms::connected()
{
    qDebug() << "Connected!";
    ui->listWidget_TCP->addItem("Connected :-)");

    //socket->write("CMD 10 0C 12 14 78 64");
}


void TCPComms::disconnected()
{
    qDebug() << "Disconnected!";
    ui->listWidget_TCP->addItem("Disconnected");
}


void TCPComms::bytesWritten (qint64 bytes)
{
    qDebug() << "we wrote: " << bytes;
    ui->listWidget_TCP->addItem("Bytes Written = " + QString::number(bytes, 10));
}


void TCPComms::readyRead()
{
    qDebug() << "Reading...";
    QByteArray rX;
    rX = socket->readAll();
    qDebug() << rX;
    ui->listWidget_TCP->addItem("Error");
}

void TCPComms::sendData(QByteArray data)
{
    if (socket->isOpen() )socket->write(data);
    ui->listWidget_TCP->addItem(data);
    //qDebug() << "TCPComms::sendData()\t " << data;
}

void TCPComms::on_btn_Connect_clicked()
{
    //Test();
}
