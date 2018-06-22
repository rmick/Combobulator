#ifndef OTAWINDOW_H
#define OTAWINDOW_H

#include <QDialog>
#include <QTimer>
#include "LttoComms.h"

namespace Ui {
class OtaWindow;
}

class OtaWindow : public QDialog
{
	Q_OBJECT

public:
	explicit OtaWindow(QWidget *parent = 0);
	~OtaWindow();

private slots:

	void on_btn_Ok_clicked();

	void on_btn_cancel_clicked();

	void on_btn_Ping_clicked();

	void pongReceived(QString pongText);

private:
	Ui::OtaWindow	*ui;
	LttoComms		*lttoComms;

	QString			ssidText;
	QString			pswdText;
};

#endif // OTAWINDOW_H
