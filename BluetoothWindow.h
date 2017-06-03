#ifndef BLUETOOTHWINDOW_H
#define BLUETOOTHWINDOW_H

#include <QDialog>
#include <qbluetoothuuid.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothservicediscoveryagent.h>

namespace Ui {
class BluetoothWindow;
}

class BluetoothWindow : public QDialog
{
    Q_OBJECT

public:
    explicit BluetoothWindow(QWidget *parent = 0);
    ~BluetoothWindow();

private:
    Ui::BluetoothWindow *ui;
};

#endif // BLUETOOTHWINDOW_H
