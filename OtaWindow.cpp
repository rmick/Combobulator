#include "OtaWindow.h"
#include "ui_OtaWindow.h"
#include <QMessageBox>

OtaWindow::OtaWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OtaWindow)
{
	ui->setupUi(this);
	lttoComms = LttoComms::getInstance();
	ssidText = "";
	pswdText = "";

	//Force the TCP connection to open.
	lttoComms->sendPacket(BEACON, 0);
	qDebug() << "Tickling";
}

OtaWindow::~OtaWindow()
{
	delete ui;
}

void OtaWindow::on_btn_Ok_clicked()
{
	ssidText = ui->lineEdit_ssidName->text();
	pswdText = ui->lineEdit_password->text();

	qDebug() << "OtaWindow::on_btn_Ok_clicked()" << ssidText << pswdText;


	if (ssidText == "")
	{
		QMessageBox::critical(this,"Error", "Network name is blank.");
		return;
	}

	if (pswdText == "")
	{
		QMessageBox::critical(this,"Error", "No password entered.");
		return;
	}

	lttoComms->sendOTAtext(ssidText, pswdText);
	lttoComms->nonBlockingDelay(2000);
	QMessageBox::warning(this,"Ready", "Please reset the Combobulator to start the update.");
	deleteLater();
}

void OtaWindow::on_btn_cancel_clicked()
{
	deleteLater();
}
