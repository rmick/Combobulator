#ifndef LTTOMAINWINDOW_H
#define LTTOMAINWINDOW_H

#include <QMainWindow>
#include "Defines.h"
#include "PlayersWindow.h"
#include "HostGameWindow.h"
#include "FlagsWindow.h"
#include "LttoComms.h"

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

    int getNumberOfTeams() const;
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

    void on_actionUSB_Serial_triggered();

    void on_actionWi_Fi_triggered();

    void on_btn_Flags_clicked();

    void on_actionAbout_triggered();

private:
    Ui::LttoMainWindow  *ui;
    PlayersWindow       *playersWindow;
    HostGameWindow      *hostGameWindow;
    FlagsWindow         *flagsWindow;

    bool    SlowTags;
    bool    MedicMode;
    bool    TeamTags;
    bool    serialUSBactive;
    bool    tcpCommsActive;

    void    UpdateGlobalPlayerControlSettings();
    void    UpdateGameControlSettings();
    void    saveFile();
    void    loadFile();
};

#endif // LTTOMAINWINDOW_H
