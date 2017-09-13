#include "FlagsWindow.h"
#include "ui_FlagsWindow.h"

FlagsWindow::FlagsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FlagsWindow)
{
    ui->setupUi(this);
}

FlagsWindow::~FlagsWindow()
{
    delete ui;
}

void FlagsWindow::setButtonStates()
{

    if(playerInfo[0].getBitFlags1(NEUTRALISE_10_FLAG))              ui->btn_Neutralise->setChecked(true);
    else                                                            ui->btn_Neutralise->setChecked(false);

    if(playerInfo[0].getBitFlags2(NEUTRALISE_15s_TAGGED_FLAG))      ui->btn_NeutraliseWhenTagged->setChecked(true);
    else                                                            ui->btn_NeutraliseWhenTagged->setChecked(false);

    if(playerInfo[0].getBitFlags2(CONTESTED_ZONES_FLAG))            ui->btn_ContestedZones->setChecked(true);
    else                                                            ui->btn_ContestedZones->setChecked(false);

    if(playerInfo[0].getBitFlags2(ZONES_TO_TEAMS_FLAG))             ui->btn_ZonesWithTeams->setChecked(true);
    else                                                            ui->btn_ZonesWithTeams->setChecked(false);

    if(playerInfo[0].getBitFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG))   ui->btn_SupplyZonesRefillTags->setChecked(true);
    else                                                            ui->btn_SupplyZonesRefillTags->setChecked(false);

    if(playerInfo[0].getBitFlags2(SUPPLY_ZONES_REVIVE_FLAG))        ui->btn_SupplyZonesReSpawn->setChecked(true);
    else                                                            ui->btn_SupplyZonesReSpawn->setChecked(false);

    if(playerInfo[0].getBitFlags2(HOSTILE_ZONES_FLAG))              ui->btn_HostileZones->setChecked(true);
    else                                                            ui->btn_HostileZones->setChecked(false);

    if(playerInfo[0].getBitFlags1(HUNT_THE_PREY_FLAG))              ui->btn_HuntThePrey->setChecked(true);
    else                                                            ui->btn_HuntThePrey->setChecked(false);

    if(playerInfo[0].getBitFlags1(REVERSE_HUNT_DIR_FLAG))           ui->btn_ReverseHuntDirection->setChecked(true);
    else                                                            ui->btn_ReverseHuntDirection->setChecked(false);
}

void FlagsWindow::updatePlayerFlags1(int bit, bool state, int firstPlayer, int lastPlayer)
{
    for(int x = firstPlayer; x <= lastPlayer; x++)
    {
        playerInfo[x].setBitFlags1(bit, state);
    }
}

void FlagsWindow::updatePlayerFlags2(int bit, bool state, int firstPlayer, int lastPlayer)
{
    for(int x = firstPlayer; x <= lastPlayer; x++)
    {
        playerInfo[x].setBitFlags2(bit, state);
    }
}

void FlagsWindow::on_btn_Neutralise_clicked()
{
    if(ui->btn_Neutralise->isChecked())             updatePlayerFlags1(NEUTRALISE_10_FLAG, true);
    else                                            updatePlayerFlags1(NEUTRALISE_10_FLAG, false);
}

void FlagsWindow::on_btn_ContestedZones_clicked()
{
    if(ui->btn_ContestedZones->isChecked())         updatePlayerFlags2(CONTESTED_ZONES_FLAG, true);
    else                                            updatePlayerFlags2(CONTESTED_ZONES_FLAG, false);
}

void FlagsWindow::on_btn_ZonesWithTeams_clicked()
{
    if(ui->btn_ZonesWithTeams->isChecked())         updatePlayerFlags2(ZONES_TO_TEAMS_FLAG, true);
    else                                            updatePlayerFlags2(ZONES_TO_TEAMS_FLAG, false);
}

void FlagsWindow::on_btn_NeutraliseWhenTagged_clicked()
{
    if(ui->btn_NeutraliseWhenTagged->isChecked())   updatePlayerFlags2(NEUTRALISE_15s_TAGGED_FLAG, true);
    else                                            updatePlayerFlags2(NEUTRALISE_15s_TAGGED_FLAG, false);
}

void FlagsWindow::on_btn_SupplyZonesRefillTags_clicked()
{
    if(ui->btn_SupplyZonesRefillTags->isChecked())  updatePlayerFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG, true);
    else                                            updatePlayerFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG, false);
}

void FlagsWindow::on_btn_HostileZones_clicked()
{
    if(ui->btn_HostileZones->isChecked())           updatePlayerFlags2(HOSTILE_ZONES_FLAG, true);
    else                                            updatePlayerFlags2(HOSTILE_ZONES_FLAG, false);
}

void FlagsWindow::on_btn_ReverseHuntDirection_clicked()
{
    if(ui->btn_ReverseHuntDirection->isChecked())   updatePlayerFlags1(REVERSE_HUNT_DIR_FLAG, true);
    else                                            updatePlayerFlags1(REVERSE_HUNT_DIR_FLAG, false);
}

void FlagsWindow::on_btn_HuntThePrey_clicked()
{
    if(ui->btn_HuntThePrey->isChecked())            updatePlayerFlags1(HUNT_THE_PREY_FLAG, true);
    else                                            updatePlayerFlags1(HUNT_THE_PREY_FLAG, false);
}

void FlagsWindow::on_btn_SupplyZonesReSpawn_clicked()
{
    if(ui->btn_SupplyZonesReSpawn->isChecked())     updatePlayerFlags2(SUPPLY_ZONES_REVIVE_FLAG, true);
    else                                            updatePlayerFlags2(SUPPLY_ZONES_REVIVE_FLAG, false);
}



void FlagsWindow::on_btn_Close_clicked()
{
    close();
}
