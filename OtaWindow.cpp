#include "OtaWindow.h"
#include "ui_OtaWindow.h"
#include <QMessageBox>

OtaWindow::OtaWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OtaWindow)
{
	ui->setupUi(this);
	lttoComms = LttoComms::getInstance();
	lttoComms->initialise();
	ssidText = "";
	pswdText = "";

	ui->btn_Ok->setEnabled(false);

	timerHeartBeat = new QTimer(this);
	connect(timerHeartBeat,	SIGNAL(timeout() ),	this,	SLOT(heartBeat()) );

	//Force the TCP to close (disconnect any sessions from a game been hosted).
	lttoComms->closePorts();

	//Force the TCP connection to open.
	timerHeartBeat->start(750);
}

OtaWindow::~OtaWindow()
{
	delete ui;
}

void OtaWindow::on_btn_Ok_clicked()
{
	ssidText = ui->lineEdit_ssidName->text();
	pswdText = ui->lineEdit_password->text();

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
	lttoComms->nonBlockingDelay(500);
	QMessageBox::warning(this,"Ready", "Please standby. The device will now restart.");
	timerHeartBeat->stop();
	deleteLater();
}

void OtaWindow::on_btn_cancel_clicked()
{
	timerHeartBeat->stop();
	close();
	lttoComms->nonBlockingDelay(1000);
	deleteLater();
}

void OtaWindow::heartBeat()
{
	static bool toggle = true;

	lttoComms->sendHeartBeat();
	qDebug() << "OtaWindow::heartBeat()";
	if(lttoComms->getTcpCommsConnected())
	{
		ui->btn_Ok->setEnabled(true);
		ui->label_Connecting->setText("");

	}
	else
	{
		ui->btn_Ok->setEnabled(false);

		if (toggle)
		{
			toggle = !toggle;
			ui->label_Connecting->setText("");
		}
		else
		{
			toggle = !toggle;
			ui->label_Connecting->setText("<html><head/><body><p><span style= font-size:48pt; font-weight:600;>Connecting to Base Station</span></p></body></html>");
		}
	}
}
