#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Game.h"
#include "Defines.h"
#include <QDebug>

SettingsWindow::SettingsWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsWindow)
{
	ui->setupUi(this);

	if(gameInfo.getIsLTARGame())		ui->radioButton_LtarMode	->setChecked(true);
	else								ui->radioButton_NormalMode	->setChecked(true);

	if(gameInfo.getIsIndoorViewMode())	ui->radioButton_IndoorMode	->setChecked(true);
	else								ui->radioButton_OutdoorMode	->setChecked(true);
	qDebug() << "SettingsWindow::SettingsWindow() - isIndoorViewMode =" << gameInfo.getIsIndoorViewMode();

	int timeOutValue = gameInfo.getCountDownTime();
	ui->sldr_CountDownTime->setValue(timeOutValue);
	//ui->label_CountDownTime->setText(QString::number(timeOutValue));
	qDebug() << timeOutValue << gameInfo.getCountDownTime();

	ui->btn_About->setText("About\n(" + VERSION_NUMBER + "-" + BUILD_NUMBER + ")");
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

void SettingsWindow::on_btn_ShutDown_clicked()
{
	ui->btn_About				->setEnabled(false);
	ui->btn_AdjustScoring		->setEnabled(false);
	ui->btn_Close				->setEnabled(false);
	ui->btn_LoadFile			->setEnabled(false);
	ui->btn_SaveFile			->setEnabled(false);
	ui->btn_ShutDown			->setEnabled(false);
	ui->sldr_CountDownTime		->setEnabled(false);
	ui->frame					->setEnabled(false);
	ui->frame_2					->setEnabled(false);
	ui->radioButton_IndoorMode	->setEnabled(false);
	ui->radioButton_OutdoorMode	->setEnabled(false);
	ui->radioButton_LtarMode	->setEnabled(false);
	ui->radioButton_NormalMode	->setEnabled(false);
	emit exitApp();
}
