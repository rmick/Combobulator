//#include "PortConnect.h"
//#include "ui_PortConnect.h"

//#include  <QDebug>
//#include "SerialComms.h"

//PortConnect::PortConnect(QWidget *parent) :
//    QDialog(parent),
//    ui(new Ui::PortConnect)
//{
//    ui->setupUi(this);

//    findserialPorts();
//    //serialComms.serialUSB = new QSerialPort();
//}

//PortConnect::~PortConnect()
//{
//    delete ui;
//}

//void PortConnect::findserialPorts()
//{
//    portNum = 0;
//    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
//    {
//        QString portFound = QObject::tr("Port:") + info.portName() + "\n"
//                    + QObject::tr("Location:") + info.systemLocation() + "\n"
//                    + QObject::tr("Description:") + info.description() + "\n"
//                    + QObject::tr("Manufacturer:") + info.manufacturer() + "\n"
//                    + QObject::tr("Serial number:") + info.serialNumber() + "\n"
//                    + QObject::tr("Vendor Identifier:") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
//                    + QObject::tr("Product Identifier:") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
//                    + QObject::tr("Busy:") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

//       //if (info.manufacturer().contains("Spark") || info.manufacturer().contains("Arduino") ) ui->comboBox_AvailablePorts->addItem(info.manufacturer() + ": " + info.portName());
//       ui->comboBox_AvailablePorts->addItem(info.manufacturer() + ": " + info.portName());
//       qDebug() << portFound;
//       qDebug() << "\n-----------\nPortToUse=";
//       portToUse[portNum++] = info;
//       if (portNum > 24) break;
//    }
//}

///*
////void SerialComms::findAndOpenSerialPort()
////{
////    QSerialPortInfo portToUse;
////    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
////    {
////        QString s = QObject::tr("Port:") + info.portName() + "\n"
////                    + QObject::tr("Location:") + info.systemLocation() + "\n"
////                    + QObject::tr("Description:") + info.description() + "\n"
////                    + QObject::tr("Manufacturer:") + info.manufacturer() + "\n"
////                    + QObject::tr("Serial number:") + info.serialNumber() + "\n"
////                    + QObject::tr("Vendor Identifier:") + (info.hasVendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString()) + "\n"
////                    + QObject::tr("Product Identifier:") + (info.hasProductIdentifier() ? QString::number(info.productIdentifier(), 16) : QString()) + "\n"
////                    + QObject::tr("Busy:") + (info.isBusy() ? QObject::tr("Yes") : QObject::tr("No")) + "\n";

////        if(!info.isBusy() && (info.description().contains("Arduino") || info.manufacturer().contains("Arduino")||
////                              info.description().contains("freetronics") || info.manufacturer().contains("SparkFun"))
////            portToUse = info;
////        qDebug() << s;
////    }

////    if(portToUse.isNull() || !portToUse.isValid())
////    {
////        qDebug() << "port is not valid:" << portToUse.portName();
////        return;
////    }


////    //    SettingsDialog::Settings p = settings->settings();
////    // Enumerate the serial port
////    // Find one that sounds like Arduino, or the highest one on the list
////    // Open it if it isn't busy

////    serialUSB->setPortName(portToUse.portName());
////    serialUSB->setBaudRate(QSerialPort::Baud115200);
////    serialUSB->setDataBits(QSerialPort::Data8);
////    serialUSB->setParity(QSerialPort::NoParity);
////    serialUSB->setStopBits(QSerialPort::OneStop);
////    serialUSB->setFlowControl(QSerialPort::NoFlowControl);
////    if (serialUSB->open(QIODevice::ReadWrite)) {
////        qDebug() << "Connected to" << portToUse.description() << "on" << portToUse.portName();
////    } else {
////        qCritical() << "Serial Port error:" << serialUSB->errorString();
////        qDebug() << "Open error";
////    }

////}
//*/





//void PortConnect::on_btn_SearchForPorts_clicked()
//{
//    findserialPorts();
//}

//void PortConnect::on_btn_Connect_clicked()
//{
//    int indexNum = -1;
//    qDebug() << ui->comboBox_AvailablePorts->currentText();
//    QString selectedPort = ui->comboBox_AvailablePorts->currentText();
//    selectedPort = selectedPort.right((selectedPort.length() - (selectedPort.indexOf(":")+2) ));
//    qDebug() << selectedPort;

//    indexNum = ui->comboBox_AvailablePorts->currentIndex();
//    //portToUse[indexNum];

//    if(portToUse[indexNum].isNull() || !portToUse[indexNum].isValid())
//    {
//        qDebug() << "port is not valid:" << portToUse[indexNum].portName();
//        return;
//    }
//    qDebug() << "Looking good";

////    serialComms.serialUSB->setPortName(portToUse[indexNum].portName());
////    serialComms.serialUSB->setBaudRate(QSerialPort::Baud115200);
////    serialComms.serialUSB->setDataBits(QSerialPort::Data8);
////    serialComms.serialUSB->setParity(QSerialPort::NoParity);
////    serialComms.serialUSB->setStopBits(QSerialPort::OneStop);
////    serialComms.serialUSB->setFlowControl(QSerialPort::NoFlowControl);
////    if (serialComms.serialUSB->open(QIODevice::ReadWrite)) {
////        qDebug() << "Connected to" << portToUse[indexNum].description() << "on" << portToUse[indexNum].portName();
////    } else {
////        qCritical() << "Serial Port error:" << serialComms.serialUSB->errorString();
////        qDebug() << "Open error";
////    }

//}

//void PortConnect::on_btn_Close_clicked()
//{
//    this->close();
//}
