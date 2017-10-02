#include "FlagsWindow.h"
#include "ui_FlagsWindow.h"
#include <QDebug>

FlagsWindow::FlagsWindow(int playerToUse, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FlagsWindow)
{
    ui->setupUi(this);
    qDebug() << "\nFlagsWindow::FlagsWindow - Using PLAYER #" << playerToUse;
    currentPlayer = playerToUse;
    setButtonStates(currentPlayer);
    if(currentPlayer != 0)
    {
        firstPlayer = currentPlayer;
        lastPlayer  = currentPlayer;
    }
    else
    {
        firstPlayer = 0;
        lastPlayer = 24;
    }
}

FlagsWindow::~FlagsWindow()
{
    delete ui;
}

////////////////////////////////////////////////////////////////////

void FlagsWindow::setButtonStates(int playerNumber)
{
    qDebug() << "FlagsWindow::setButtonStates = Player " << playerNumber;
    if(gameInfo.getIsLTARGame())    hideLtarControls(true);
    else                            hideLtarControls(false);

    if(playerInfo[playerNumber].getBitFlags1(NEUTRALISE_10_FLAG))               ui->btn_Neutralise->setChecked(true);
    else                                                                        ui->btn_Neutralise->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags1(HUNT_THE_PREY_FLAG))               ui->btn_HuntThePrey->setChecked(true);
    else                                                                        ui->btn_HuntThePrey->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags1(REVERSE_HUNT_DIR_FLAG))            ui->btn_ReverseHuntDirection->setChecked(true);
    else                                                                        ui->btn_ReverseHuntDirection->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags2(NEUTRALISE_15s_TAGGED_FLAG))       ui->btn_NeutraliseWhenTagged->setChecked(true);
    else                                                                        ui->btn_NeutraliseWhenTagged->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags2(CONTESTED_ZONES_FLAG))             ui->btn_ContestedZones->setChecked(true);
    else                                                                        ui->btn_ContestedZones->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags2(ZONES_TO_TEAMS_FLAG))              ui->btn_ZonesWithTeams->setChecked(true);
    else                                                                        ui->btn_ZonesWithTeams->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG))    ui->btn_SupplyZonesRefillTags->setChecked(true);
    else                                                                        ui->btn_SupplyZonesRefillTags->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags2(SUPPLY_ZONES_REVIVE_FLAG))         ui->btn_SupplyZonesReSpawn->setChecked(true);
    else                                                                        ui->btn_SupplyZonesReSpawn->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags2(HOSTILE_ZONES_FLAG))               ui->btn_HostileZones->setChecked(true);
    else                                                                        ui->btn_HostileZones->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(DISABLE_LTAR_ENHANCED_BEACONS))    ui->btn_LtarEnhancedBeacons->setChecked(true);
    else                                                                        ui->btn_LtarEnhancedBeacons->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(DISABLE_IFF_BEACONS))              ui->btn_LtarIFFbeacon->setChecked(true);
    else                                                                        ui->btn_LtarIFFbeacon->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(DISABLE_TAG_RECVD_BEACON))         ui->btn_LtarReceivedBeacons->setChecked(true);
    else                                                                        ui->btn_LtarReceivedBeacons->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(SHIELD_BUTTON_HOLD_MODE))          ui->btn_LtarShieldButtonMode->setChecked(true);
    else                                                                        ui->btn_LtarShieldButtonMode->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(SUPPLY_ZONES_REFILL_AMMO))         ui->btn_LtarSupplyZonesRefillAmmo->setChecked(true);
    else                                                                        ui->btn_LtarSupplyZonesRefillAmmo->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(SUPPLY_ZONES_REFILL_SHIELDS))      ui->btn_LtarSupplyZonesRefillShields->setChecked(true);
    else                                                                        ui->btn_LtarSupplyZonesRefillShields->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(AUTO_RELOAD))                      ui->btn_LtarAutoReload->setChecked(true);
    else                                                                        ui->btn_LtarAutoReload->setChecked(false);

    if(playerInfo[playerNumber].getBitFlags3(ALLOW_FULL_AUTO_FIRE))             ui->btn_LtarFullAutoMode->setChecked(true);
    else                                                                        ui->btn_LtarFullAutoMode->setChecked(false);
}

void FlagsWindow::updatePlayerFlags1(int bit, bool state)
{
    for(int index = firstPlayer; index <= lastPlayer; index++)
    {
        playerInfo[index].setBitFlags1(bit, state);
        qDebug() << "FlagsWindow::updatePlayerFlags1 - Player" << index;
    }
}

void FlagsWindow::updatePlayerFlags2(int bit, bool state)
{
    for(int index = firstPlayer; index <= lastPlayer; index++)
    {
        playerInfo[index].setBitFlags2(bit, state);
        qDebug() << "FlagsWindow::updatePlayerFlags2 - Player" << index;
    }
}

void FlagsWindow::updatePlayerFlags3(int bit, bool state)
{
    for(int index = firstPlayer; index <= lastPlayer; index++)
    {
        playerInfo[index].setBitFlags3(bit, state);
        qDebug() << "FlagsWindow::updatePlayerFlags3 - Player" << index;
    }
}

void FlagsWindow::hideLtarControls(bool state)
{
    ui->btn_LtarAutoReload->setVisible(state);
    ui->btn_LtarEnhancedBeacons->setVisible(state);
    ui->btn_LtarFullAutoMode->setVisible(state);
    ui->btn_LtarIFFbeacon->setVisible(state);
    ui->btn_LtarReceivedBeacons->setVisible(state);
    ui->btn_LtarShieldButtonMode->setVisible(state);
    ui->btn_LtarSupplyZonesRefillAmmo->setVisible(state);
    ui->btn_LtarSupplyZonesRefillShields->setVisible(state);
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
    if(ui->btn_SupplyZonesRefillTags->isChecked())      updatePlayerFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG, true);
    else                                                updatePlayerFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG, false);
}

void FlagsWindow::on_btn_HostileZones_clicked()
{
    if(ui->btn_HostileZones->isChecked())               updatePlayerFlags2(HOSTILE_ZONES_FLAG, true);
    else                                                updatePlayerFlags2(HOSTILE_ZONES_FLAG, false);
}

void FlagsWindow::on_btn_ReverseHuntDirection_clicked()
{
    if(ui->btn_ReverseHuntDirection->isChecked())       updatePlayerFlags1(REVERSE_HUNT_DIR_FLAG, true);
    else                                                updatePlayerFlags1(REVERSE_HUNT_DIR_FLAG, false);
}

void FlagsWindow::on_btn_HuntThePrey_clicked()
{
    if(ui->btn_HuntThePrey->isChecked())                updatePlayerFlags1(HUNT_THE_PREY_FLAG, true);
    else                                                updatePlayerFlags1(HUNT_THE_PREY_FLAG, false);
}

void FlagsWindow::on_btn_SupplyZonesReSpawn_clicked()
{
    if(ui->btn_SupplyZonesReSpawn->isChecked())         updatePlayerFlags2(SUPPLY_ZONES_REVIVE_FLAG, true);
    else                                                updatePlayerFlags2(SUPPLY_ZONES_REVIVE_FLAG, false);
}

void FlagsWindow::on_btn_LtarEnhancedBeacons_clicked()
{
    if(ui->btn_LtarEnhancedBeacons->isChecked())        updatePlayerFlags3(DISABLE_LTAR_ENHANCED_BEACONS, true);
    else                                                updatePlayerFlags3(DISABLE_LTAR_ENHANCED_BEACONS, false);
}

void FlagsWindow::on_btn_LtarReceivedBeacons_clicked()
{
    if(ui->btn_LtarReceivedBeacons->isChecked())        updatePlayerFlags3(DISABLE_TAG_RECVD_BEACON, true);
    else                                                updatePlayerFlags3(DISABLE_TAG_RECVD_BEACON, false);
}

void FlagsWindow::on_btn_LtarIFFbeacon_clicked()
{
    if(ui->btn_LtarIFFbeacon->isChecked())              updatePlayerFlags3(DISABLE_IFF_BEACONS, true);
    else                                                updatePlayerFlags3(DISABLE_IFF_BEACONS, false);
}

void FlagsWindow::on_btn_LtarSupplyZonesRefillAmmo_clicked()
{
    if(ui->btn_LtarSupplyZonesRefillAmmo->isChecked())  updatePlayerFlags3(SUPPLY_ZONES_REFILL_AMMO, true);
    else                                                updatePlayerFlags3(SUPPLY_ZONES_REFILL_AMMO, false);
}

void FlagsWindow::on_btn_LtarSupplyZonesRefillShields_clicked()
{
    if(ui->btn_LtarSupplyZonesRefillShields->isChecked())   updatePlayerFlags3(SUPPLY_ZONES_REFILL_SHIELDS, true);
    else                                                    updatePlayerFlags3(SUPPLY_ZONES_REFILL_SHIELDS, false);
}

void FlagsWindow::on_btn_LtarShieldButtonMode_clicked()
{
    if(ui->btn_LtarShieldButtonMode->isChecked())           updatePlayerFlags3(SHIELD_BUTTON_HOLD_MODE, true);
    else                                                    updatePlayerFlags3(SHIELD_BUTTON_HOLD_MODE, false);
}

void FlagsWindow::on_btn_LtarFullAutoMode_clicked()
{
    if(ui->btn_LtarFullAutoMode->isChecked())               updatePlayerFlags3(ALLOW_FULL_AUTO_FIRE, true);
    else                                                    updatePlayerFlags3(ALLOW_FULL_AUTO_FIRE, false);
}

void FlagsWindow::on_btn_LtarAutoReload_clicked()
{
    if(ui->btn_LtarAutoReload->isChecked())                 updatePlayerFlags3(AUTO_RELOAD, true);
    else                                                    updatePlayerFlags3(AUTO_RELOAD, false);
}

void FlagsWindow::on_btn_Close_clicked()
{
    deleteLater();
}





