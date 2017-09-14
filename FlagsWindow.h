#ifndef FLAGSWINDOW_H
#define FLAGSWINDOW_H

#include <QDialog>
#include "Defines.h"
#include "Players.h"
#include "Game.h"

namespace Ui {
class FlagsWindow;
}

class FlagsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FlagsWindow(int playerToUse, QWidget *parent = 0);
    ~FlagsWindow();

    void setButtonStates();

private slots:
    void on_btn_Neutralise_clicked();

    void on_btn_ContestedZones_clicked();

    void on_btn_ZonesWithTeams_clicked();

    void on_btn_NeutraliseWhenTagged_clicked();

    void on_btn_SupplyZonesRefillTags_clicked();

    void on_btn_HostileZones_clicked();

    void on_btn_ReverseHuntDirection_clicked();

    void on_btn_Close_clicked();

    void on_btn_HuntThePrey_clicked();

    void on_btn_SupplyZonesReSpawn_clicked();

    void on_btn_LtarEnhancedBeacons_clicked();

    void on_btn_LtarReceivedBeacons_clicked();

    void on_btn_LtarIFFbeacon_clicked();

    void on_btn_LtarSupplyZonesRefillAmmo_clicked();

    void on_btn_LtarSupplyZonesRefillShields_clicked();

    void on_btn_LtarShieldButtonMode_clicked();

    void on_btn_LtarFullAutoMode_clicked();

    void on_btn_LtarAutoReload_clicked();

private:
    Ui::FlagsWindow *ui;
    void updatePlayerFlags1(int bit, bool state);
    void updatePlayerFlags2(int bit, bool state);
    void updatePlayerFlags3(int bit, bool state);

    void hideLtarControls(bool state);

    int currentPlayer;
    int firstPlayer;
    int lastPlayer;

};

#endif // FLAGSWINDOW_H
