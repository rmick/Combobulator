#ifndef LTTOMAINWINDOW_H
#define LTTOMAINWINDOW_H

#include <QMainWindow>
#include "PlayersWindow.h"
#include "BluetoothWindow.h"
#include "HostGameWindow.h"
#include "PortConnect.h"

namespace Ui {
class LttoMainWindow;
}

class LttoMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LttoMainWindow(QWidget *parent = 0);
    ~LttoMainWindow();

private:


public:
    struct PlayerStructure
    {
        int     PlayerID;
        QString PlayerName;
        int     HealthNumberofTags;
        int     ShieldsTime;
        int     MegaCount;
        bool    SlowTagsEnabled;
        bool    MediModeEnabled;
        bool    TeamTagsEnabled;
        int     TeamAssignment;
        int     Handicap;
    };

    bool getSlowTags() const;
    void setSlowTags(bool value);

    bool getMedicMode() const;
    void setMedicMode(bool value);

    bool getTeamTags() const;
    void setTeamTags(bool value);

    int getNumberOfTeams() const;
    void setNumberOfTeams(int value);

    //HostGameWindow      *hostGameWindow;

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

    void on_btn_Quit_clicked();

    void on_btn_Spies_clicked();

    void on_btnSave_clicked();

    void on_btnLoad_clicked();

    void on_btn_Bluetooth_clicked();

    void on_actionExit_triggered();

    void on_btn_CustomGame_clicked();

    void on_actionPorts_triggered();

    void on_actionSet_CountDown_Time_triggered();

private:
    Ui::LttoMainWindow  *ui;
    PlayersWindow       *playersWindow;
    BluetoothWindow     *bluetoothWindow;
    HostGameWindow      *hostGameWindow;
    PortConnect         *portConnect;

    bool    SlowTags;
    bool    MedicMode;
    bool    TeamTags;

    void    UpdateGlobalPlayerControlSettings();
    void    UpdateGameControlSettings();
};

#endif // LTTOMAINWINDOW_H
