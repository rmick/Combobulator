#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Game.h"
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsWindow)
{
	ui->setupUi(this);

	if(gameInfo.getIsLTARGame())		ui->radioButton_LtarMode->setChecked(true);
	else								ui->radioButton_NormalMode->setChecked(true);

	if(gameInfo.getIsIndoorViewMode())	ui->radioButton_IndoorMode->setChecked(true);
	else								ui->radioButton_OutdoorMode->setChecked(true);

	int timeOutValue = gameInfo.getCountDownTime();
	ui->sldr_CountDownTime->setValue(timeOutValue);
	//ui->sldr_CountDownTime->setSliderPosition(timeOutValue);
	ui->label_CountDownTime->setText(QString::number(timeOutValue));
	qDebug() << timeOutValue << gameInfo.getCountDownTime();
}

SettingsWindow::~SettingsWindow()
{
	delete ui;
}

void SettingsWindow::on_btn_Close_clicked()
{
	delete(this);
}

void SettingsWindow::on_btn_AdjustScoring_clicked()
{
	emit adjustScoring();
}

void SettingsWindow::on_btn_LoadFile_clicked()
{
	emit loadFile();
}

void SettingsWindow::on_btn_SaveFile_clicked()
{
	emit saveFile();
}

void SettingsWindow::on_btn_About_clicked()
{
	emit showAboutBox();
}

void SettingsWindow::on_radioButton_NormalMode_clicked()
{
	emit setLtarMode(false);
}

void SettingsWindow::on_radioButton_LtarMode_clicked()
{
	emit setLtarMode(true);
}

void SettingsWindow::on_radioButton_IndoorMode_clicked()
{
	emit setOutdoorMode(false);
}

void SettingsWindow::on_radioButton_OutdoorMode_clicked()
{
	emit setOutdoorMode(true);
}

void SettingsWindow::on_sldr_CountDownTime_valueChanged(int value)
{
	//gameInfo.setCountDownTime(value);
	qDebug() << "SettingsWindow::on_sldr_CountDownTime_valueChanged(int value)" << value;
	ui->label_CountDownTime->setText(QString::number(value));
	gameInfo.setCountDownTime(value);
}
