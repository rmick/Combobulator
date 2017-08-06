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
    if(ui->btn_Neutralise->isChecked()) updatePlayerFlags1(7, true);
    else                                updatePlayerFlags1(7, false);
}

void FlagsWindow::on_btn_ContestedZones_clicked()
{
    if(ui->btn_ContestedZones->isChecked()) updatePlayerFlags2(7, true);
    else                                    updatePlayerFlags2(7, false);
}

void FlagsWindow::on_btn_ZonesWithTeams_clicked()
{
    if(ui->btn_ZonesWithTeams->isChecked()) updatePlayerFlags2(6, true);
    else                                    updatePlayerFlags2(6, false);
}

void FlagsWindow::on_btn_NeutraliseWhenTagged_clicked()
{
    if(ui->btn_NeutraliseWhenTagged->isChecked()) updatePlayerFlags2(5, true);
    else                                          updatePlayerFlags2(5, false);
}

void FlagsWindow::on_btn_SupplyZonesRefillTags_clicked()
{
    if(ui->btn_SupplyZonesRefillTags->isChecked()) updatePlayerFlags2(4, true);
    else                                           updatePlayerFlags2(4, false);
}

void FlagsWindow::on_btn_HostileZones_clicked()
{
    if(ui->btn_HostileZones->isChecked()) updatePlayerFlags2(3, true);
    else                                  updatePlayerFlags2(3, false);
}

void FlagsWindow::on_btn_ReverseHuntDirection_clicked()
{
    if(ui->btn_ReverseHuntDirection->isChecked()) updatePlayerFlags1(0, true);
    else                                          updatePlayerFlags1(0, false);
}

void FlagsWindow::on_btn_Close_clicked()
{
    close();
}
