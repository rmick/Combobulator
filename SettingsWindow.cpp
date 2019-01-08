#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Game.h"
#include "Defines.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

SettingsWindow::SettingsWindow(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsWindow)
{
	ui->setupUi(this);

	if(gameInfo.getIsLTARGame())			ui->radioButton_LtarMode	->setChecked(true);
	else									ui->radioButton_NormalMode	->setChecked(true);

	if(gameInfo.getIsIndoorViewMode())		ui->radioButton_IndoorMode	->setChecked(true);
	else									ui->radioButton_OutdoorMode	->setChecked(true);
	qDebug() << "SettingsWindow::SettingsWindow() - isIndoorViewMode =" << gameInfo.getIsIndoorViewMode();

	if(gameInfo.getCumulativeScoreMode())	ui->btn_CumulativeScores	->setChecked(true);
	else									ui->btn_CumulativeScores	->setChecked(false);

	int timeOutValue = gameInfo.getCountDownTime();
	ui->sldr_CountDownTime->setValue(timeOutValue);

	ui->btn_About->setText("About\n" + VERSION_NUMBER + " - " + BUILD_NUMBER);

	//ui->btn_CumulativeScores->setVisible(false);
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
	qInfo() << "SettingsWindow::on_radioButton_NormalMode_clicked() - Normal Mode Active";
}

void SettingsWindow::on_radioButton_LtarMode_clicked()
{
	emit setLtarMode(true);
	qInfo() << "SettingsWindow::on_radioButton_LtarMode_clicked() - LTAR Mode Active";
}

void SettingsWindow::on_radioButton_IndoorMode_clicked()
{
	emit setOutdoorMode(false);
	qDebug() << "TimerStart";
	QTimer::singleShot(100, this, SLOT(refreshDisplay( )));
}


void SettingsWindow::on_radioButton_OutdoorMode_clicked()
{
	emit setOutdoorMode(true);
	qDebug() << "TimerStart";
	QTimer::singleShot(100, this, SLOT(refreshDisplay( )));
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

void SettingsWindow::refreshDisplay()
{
	qDebug() << "Updated";
	QWidget::update();
}

void SettingsWindow::on_btn_CumulativeScores_clicked()
{
	if(ui->btn_CumulativeScores->isChecked())	gameInfo.setCumulativeScoreMode(true);
	else										gameInfo.setCumulativeScoreMode(false);

	if(gameInfo.getCumulativeScoreMode())	qInfo() << "SettingsWindow::on_btn_CumulativeScores_clicked() - Cumulative Scoring is Active";
	else									qInfo() << "SettingsWindow::on_btn_CumulativeScores_clicked() - Cumulative Scoring is Disabled";
}

void SettingsWindow::on_btn_DoNotPressThisButton_clicked()
{
	//I always wanted to add this homage to HHGG into a product, so here it is.
	QMessageBox::critical(this,"WARNING","Please do not press this button again.");
}
