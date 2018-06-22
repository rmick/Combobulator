#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
	Q_OBJECT

public:
	explicit SettingsWindow(QWidget *parent = 0);
	~SettingsWindow();

private slots:
	void on_btn_Close_clicked();

	void on_btn_AdjustScoring_clicked();

	void on_sldr_CountDownTime_valueChanged(int value);

	void on_btn_LoadFile_clicked();

	void on_btn_SaveFile_clicked();

	void on_btn_About_clicked();

	void on_radioButton_NormalMode_clicked();

	void on_radioButton_LtarMode_clicked();

	void on_radioButton_IndoorMode_clicked();

	void on_radioButton_OutdoorMode_clicked();

	void on_btn_ShutDown_clicked();

signals:
	void	adjustScoring();
	void	saveFile();
	void	loadFile();
	void	setOutdoorMode(bool state);
	void	setLtarMode(bool state);
	void	showAboutBox();
	void	exitApp();

private:
	Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
