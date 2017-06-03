#include "BluetoothWindow.h"
#include "ui_BluetoothWindow.h"

BluetoothWindow::BluetoothWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BluetoothWindow)
{
    ui->setupUi(this);
}

BluetoothWindow::~BluetoothWindow()
{
    delete ui;
}

//QBluetoothLocalDevice localDevice;
//QString localDeviceName;

//void InitBT()
//{
//    // Check if Bluetooth is available on this device
//    if (localDevice.isValid())
//    {
//        // Turn Bluetooth on
//        localDevice.powerOn();

//        // Read local device name
//        localDeviceName = localDevice.name();

//        // Make it visible to others
//        localDevice.setHostMode(QBluetoothLocalDevice::HostDiscoverable);

//        // Get connected devices
//        QList<QBluetoothAddress> remotes;
//        remotes = localDevice.connectedDevices();
//    }
//}

