#ifndef LTTOMAINWINDOW_H
#define LTTOMAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QSoundEffect>
#include "Defines.h"
#include "PlayersWindow.h"
#include "HostGameWindow.h"
#include "FlagsWindow.h"
#include "LttoComms.h"
#include "AboutForm.h"
#include "ScoresWindow.h"
#include "SetScoreParameters.h"
#include "FileLoadSave.h"
#include "OtaWindow.h"

namespace Ui {
class LttoMainWindow;
}

class LttoMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LttoMainWindow(QWidget *parent = 0);
    ~LttoMainWindow();

    bool getSlowTags() const;
    void setSlowTags(bool value);

    bool getMedicMode() const;
    void setMedicMode(bool value);

    bool getTeamTags() const;
    void setTeamTags(bool value);

    int  getNumberOfTeams() const;
    void setNumberOfTeams(int value);

public slots:
    void SetSpiesButtonState(int NumTeams);

private slots:

    void on_btn_SelectPlayers_clicked();

    void on_btn_SlowTags_clicked();

    void on_btn_MedicMode_clicked();

    void on_btn_StartGame_clicked();

    void on_btn_TeamTags_clicked();

    void on_btn_NoTeams_clicked();

    void on_btn_HideAndSeek_clicked();

    void on_btn_Ltag_clicked();

    void on_btn_Kings_clicked();

    void on_btn_OwnTheZone_clicked();

    void on_btn_TwoTeams_clicked();

    void on_btn_ThreeTeams_clicked();

    void on_slider_Reloads_valueChanged(int value);

    void on_slider_Health_valueChanged(int value);

    void on_slider_Shields_valueChanged(int value);

    void on_slider_MegaTags_valueChanged(int value);

    void on_slider_GameTime_valueChanged(int value);

    void on_btn_Spies_clicked();

    void on_actionExit_triggered();

    void on_btn_CustomGame_clicked();

    void on_actionSet_CountDown_Time_triggered();

    void on_actionuse_LazerSwarm_triggered();

    void on_actionSave_triggered();

    void on_actionLoad_triggered();

    void on_btn_Flags_clicked();

    void on_actionAbout_triggered();

    void on_btn_SpyTeamTags_clicked();

    void on_slider_StartAmmo_valueChanged(int value);

    void on_slider_SleepTime_valueChanged(int value);

    void on_actionLTAR_Mode_triggered();

    void on_btn_Debug_clicked();

    void on_actionOutdoorMode_triggered();

    void on_btn_ReSpawn_clicked();

    void on_btn_SetScorePoints_clicked();

	void updateFileName(QString newFileName);

	void on_actionEdit_Scoring_triggered();

	void on_actionUpdate_Firmware_triggered();

private:
    Ui::LttoMainWindow      *ui;
	QPointer<PlayersWindow>			playersWindow;
	QPointer<HostGameWindow>		hostGameWindow;
	QPointer<FlagsWindow>			flagsWindow;
	QPointer<ScoresWindow>			scoresWindow;
	QPointer<FileLoadSave>			fileLoadSave;
	QPointer<SetScoreParameters>	setScoreParameters;
	QPointer<OtaWindow>				otaWindow;
	QPointer<AboutForm>             aboutForm;
	QSoundEffect					*sound_PowerUp;
	QSoundEffect					*sound_Powerdown;
	LttoComms						*lttoComms;

    bool    SlowTags;
    bool    MedicMode;
    bool    TeamTags;
	QString	fileLoadSaveName;	// Updated by SIGNAL from FileLoadSave, used by FileSave.

    void    UpdateGlobalPlayerControlSettings();
    void    UpdateGameControlSettings();
    void    setLtarControls(bool state);
    void    saveFile();
    void    loadFile();
    void    loadSettings();
    void    saveSettings();

    QString   QSS_Inside =  "QWidget{background-color:black; font:white;} QSpinBox{color:white;}";
    //QSS_Inside +=           "QMessageBox{font:white;} QMenu{font:white; color:white;}";
    //QSS_INSIDE +=           "QLabel{color:white;} QListWidget{color:white;}";
    //QSS_INSIDE +=           "QToolButton{border-width:2px; border-radius:10px; border-color:cyan; background-color:grey; padding:5px; spacing:20px; font:bold;}";

    QString QSS_Outside =   "QWidget{background-color:yellow; font:purple;} QPushbutton{background-color:white;}";

    //QSS_Inside = "";
};

#endif // LTTOMAINWINDOW_H
