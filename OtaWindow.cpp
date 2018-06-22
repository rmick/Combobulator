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

	connect(lttoComms, SIGNAL(PongReceived(QString)), this, SLOT(pongReceived(QString)));
	//Force the TCP to close (disconnect any sessions from a game been hosted).
	lttoComms->closePorts();
	lttoComms->nonBlockingDelay(500);

	//Force the TCP connection to open.
	lttoComms->sendPacket(BEACON, 0);
	qDebug() << "Tickling";
	lttoComms->nonBlockingDelay(500);
	lttoComms->sendPing("Set WiFi");

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
	ui->lineEdit_ssidName->setText("");
	ui->lineEdit_password->setText("");
	ui->btn_Ok->setEnabled(false);
	ui->btn_cancel->setEnabled(false);
	//lttoComms->nonBlockingDelay(2000);
	QMessageBox::warning(this,"Ready", "Please standby. The device will now restart.");
	deleteLater();
}

void OtaWindow::on_btn_cancel_clicked()
{
	deleteLater();
}

void OtaWindow::on_btn_Ping_clicked()
{
	lttoComms->sendPing("Test Ping");
}

void OtaWindow::pongReceived(QString pongText)
{
	qDebug() << "\tOtaWindow::pongReceived()" << pongText;
}
