#include "PlayersWindow.h"
#include "ui_PlayersWindow.h"

#include <QDebug>
#include <QButtonGroup>
#include <QElapsedTimer>
#include <QInputDialog>

#include "LttoMainWindow.h"
#include "Game.h"
#include "Players.h"

PlayersWindow::PlayersWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayersWindow),
    signalMapperClicked(NULL),
    signalMapperPressed(NULL),
    signalMapperReleased(NULL)
{
    SelectedPlayer = 0;
    ui->setupUi(this);
    signalMapperClicked  = new QSignalMapper(this);
    signalMapperPressed  = new QSignalMapper(this);
    signalMapperReleased = new QSignalMapper(this);
    SetUpPlayerButtonMapping();
    SetPlayerControls(false, HANDICAP_MODE);
    LoadPlayersForTeams();
    LoadPlayerSettings(0);      // 0 = Global Player
    SetActivePlayers();
    ui->line_3->setVisible(false);
    RenamePlayerTeamButtons(gameInfo.getNumberOfTeams());
}

PlayersWindow::~PlayersWindow()
{
    delete ui;
}

/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::SetUpPlayerButtonMapping()
{
    PlayerButtons.append(0);    // Fake button for Player 0
    PlayerButtons.append(ui->btn_Player1);
    PlayerButtons.append(ui->btn_Player2);
    PlayerButtons.append(ui->btn_Player3);
    PlayerButtons.append(ui->btn_Player4);
    PlayerButtons.append(ui->btn_Player5);
    PlayerButtons.append(ui->btn_Player6);
    PlayerButtons.append(ui->btn_Player7);
    PlayerButtons.append(ui->btn_Player8);
    PlayerButtons.append(ui->btn_Player9);
    PlayerButtons.append(ui->btn_Player10);
    PlayerButtons.append(ui->btn_Player11);
    PlayerButtons.append(ui->btn_Player12);
    PlayerButtons.append(ui->btn_Player13);
    PlayerButtons.append(ui->btn_Player14);
    PlayerButtons.append(ui->btn_Player15);
    PlayerButtons.append(ui->btn_Player16);
    PlayerButtons.append(ui->btn_Player17);
    PlayerButtons.append(ui->btn_Player18);
    PlayerButtons.append(ui->btn_Player19);
    PlayerButtons.append(ui->btn_Player20);
    PlayerButtons.append(ui->btn_Player21);
    PlayerButtons.append(ui->btn_Player22);
    PlayerButtons.append(ui->btn_Player23);
    PlayerButtons.append(ui->btn_Player24);

    //Connect button array to SignalMappers
    for (int x = 1; x<25; x++)
    {
        connect (PlayerButtons[x],  SIGNAL(pressed()),  signalMapperPressed,  SLOT (map()) );
        connect (PlayerButtons[x],  SIGNAL(released()), signalMapperReleased, SLOT (map()) );
        signalMapperPressed ->setMapping(PlayerButtons[x] ,  x);
        signalMapperReleased->setMapping(PlayerButtons[x] ,  x);
    }
    connect (signalMapperPressed,  SIGNAL(mapped(int)), this, SLOT(playerButtonPressed(int))   );
    connect (signalMapperReleased, SIGNAL(mapped(int)), this, SLOT(playerButtonReleased(int))   );
}

/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::LoadPlayersForTeams()
{
    switch (gameInfo.getNumberOfTeams() )
    {
        case 0:
        case 1:
            for (int x = 17; x<25; x++)
            {
                PlayerButtons[x]->setVisible(true);
            }
            break;
        case 2:
            for (int x = 17; x<25; x++)
            {
                PlayerButtons[x]->setVisible(false);
            }
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::LoadPlayerSettings(int PlayerID)
{
    // Index 0 is the global setting.

    ui->slider_Health               ->setValue  (playerInfo[PlayerID].getHealthTags() );
    ui->slider_Shields              ->setValue  (playerInfo[PlayerID].getShieldTime() );
    ui->slider_MegaTags             ->setValue  (playerInfo[PlayerID].getMegaTags() );
    ui->btn_SelectedPlayerSlowTags  ->setChecked(playerInfo[PlayerID].getSlowTags() );

    if (playerInfo[PlayerID].getSlowTags() == false)    ui->btn_SelectedPlayerSlowTags  ->setText("OFF");
    else                                                ui->btn_SelectedPlayerSlowTags  ->setText("ON");

    ui->btn_SelectedPlayerTeamTags  ->setChecked(playerInfo[PlayerID].getTeamTags() );
    if (playerInfo[PlayerID].getTeamTags() == false)    ui->btn_SelectedPlayerTeamTags  ->setText("OFF");
    else                                                ui->btn_SelectedPlayerTeamTags  ->setText("ON");
    int reLoads;
    if (playerInfo[PlayerID].getReloads() == 0) reLoads = 100;
    else reLoads = playerInfo[PlayerID].getReloads();
    ui->slider_Reloads              ->setValue(reLoads);
    ui->slider_Handicap             ->setValue  (playerInfo[PlayerID].getHandicap() ); // Do this last !!!
}

void PlayersWindow::SetActivePlayers()
{
    for (int index = 1; index < 25; index++)
    {
        if      (gameInfo.getIsThisPlayerInTheGame(index) == true)  PlayerButtons[index]->setChecked(true);
        else                                                        PlayerButtons[index]->setChecked(false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::on_btn_ClosePlayerWindow_clicked()
{
    delete(this);
}

void PlayersWindow::on_btn_SelectNone_clicked()
{
    SetPlayerButtons(false);
    SetPlayerControls(false, CURRENT_MODE);
}

void PlayersWindow::on_btn_SelectAll_clicked()
{
    SetPlayerControls(false, CURRENT_MODE);
    SetPlayerButtons(true);
}

void PlayersWindow::SetPlayerButtons(bool state)
{
    for (int x = 1; x<25; x++)
    {
         PlayerButtons[x]->setChecked(state);
         //The next two lines should not be required, but the stylesheet fails when SelectAll/SelectNone are triggered.
         if (state == true) PlayerButtons[x]->setStyleSheet("font: bold;");
         else               PlayerButtons[x]->setStyleSheet("font: normal;");
         gameInfo.setIsThisPlayerInTheGame(x, state);
    }
}

void PlayersWindow::SetPlayerControls(int state, int mode)
{
    //Set the state of ALL the controls.
    if(state != CURRENT_MODE)
    {
        ui->label_Handicap->setEnabled(state);
        ui->slider_Handicap->setEnabled(state);
        ui->slider_Health->setEnabled(state);
        ui->slider_MegaTags->setEnabled(state);
        ui->slider_Reloads->setEnabled(state);
        ui->slider_Shields->setEnabled(state);
        ui->label_SlowTags->setEnabled(state);
        ui->label_TeamTags->setEnabled(state);
        ui->btn_SelectedPlayerSlowTags->setEnabled(state);
        ui->btn_SelectedPlayerTeamTags->setEnabled(state);
    }

    //Now setVisible for those we want to see
    if(mode != CURRENT_MODE)
    {
        ui->slider_Health->setVisible(mode);
        ui->slider_MegaTags->setVisible(mode);
        ui->slider_Reloads->setVisible(mode);
        ui->slider_Shields->setVisible(mode);
        ui->slider_Handicap->setVisible(!mode);
        ui->label_Handicap->setVisible(!mode);
    }
}

void PlayersWindow::setSelectedPlayer(int value)
{
    if      (gameInfo.getIsThisPlayerInTheGame(value) == false)
    {
        gameInfo.setIsThisPlayerInTheGame(value, true);
        if      (value < 9)     gameInfo.setPlayersInTeamByte(1, value,    true) ;
        else if (value < 17)    gameInfo.setPlayersInTeamByte(2, value-8,  true) ;
        else if (value < 25)    gameInfo.setPlayersInTeamByte(3, value-16, true) ;
    }
    else if (gameInfo.getIsThisPlayerInTheGame(value) == true)
    {
        gameInfo.setIsThisPlayerInTheGame(value, false);
        if      (value < 9)     gameInfo.setPlayersInTeamByte(1, value,    false) ;
        else if (value < 17)    gameInfo.setPlayersInTeamByte(2, value-8,  false) ;
        else if (value < 25)    gameInfo.setPlayersInTeamByte(3, value-16, false) ;
    }
    SelectedPlayer = value;
    SetPlayerControls(true, CURRENT_MODE);
}

void PlayersWindow::RenamePlayerTeamButtons(int numTeams)
{
    switch(numTeams)
    {
    case 0:
        for (int index = 1; index < 25; index++)
        {
            PlayerButtons[index]->setText("Player " + QString::number(index) );

            //Rename player buttons if they do NOT have the default name
            if (playerInfo[index].getPlayerName() != "Player " + QString::number(index))
            {
                PlayerButtons[index]->setText(playerInfo[index].getPlayerName());
            }
        }
        ui->label_Team1->setText("Players");
        ui->label_Team2->setVisible(false);
        ui->label_Team3->setVisible(false);
        ui->line_1->setVisible(false);
        ui->line_2->setVisible(false);
        ui->line_2->setVisible(false);
        break;
    case 2:
    case 3:
        for (int index = 1; index < 25; index++)
        {
            if      (index < 9)     PlayerButtons[index]->setText("Player " + QString::number(index) );
            else if (index < 17)    PlayerButtons[index]->setText("Player " + QString::number(index-8) );
            else if (index < 25)    PlayerButtons[index]->setText("Player " + QString::number(index-16) );

            //Rename player buttons if they do NOT have the default name
            if (playerInfo[index].getPlayerName() != "Player " + QString::number(index))
            {
                PlayerButtons[index]->setText(playerInfo[index].getPlayerName());
            }
        }
        ui->label_Team1->setText("Team 1");
//        ui->label_Team2->setText("Team 2");
//        ui->label_Team3->setText("Team 3");
        ui->line_1->setVisible(true);
        ui->line_2->setVisible(true);
        ui->label_Team1->setVisible(true);
        ui->label_Team2->setVisible(true);
        if(gameInfo.getNumberOfTeams() == 2)
        {
            ui->label_Team3->setVisible(false);
            ui->line_2->setVisible(false);
        }
        else
        {
            ui->label_Team3->setVisible(true);
            ui->line_2->setVisible(true);
        }
        break;
    }
}

int PlayersWindow::getSelectedPlayer() const
{
    return SelectedPlayer;
}

//////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::on_btn_SelectedPlayerSlowTags_clicked()
{
    if(ui->btn_SelectedPlayerSlowTags->isChecked() )
    {
        ui->btn_SelectedPlayerSlowTags->setText("ON");
        playerInfo[SelectedPlayer].setSlowTags(true);
    }
    else
    {
        ui->btn_SelectedPlayerSlowTags->setText("OFF");
        playerInfo[SelectedPlayer].setSlowTags(false);
    }
}

void PlayersWindow::on_btn_SelectedPlayerTeamTags_clicked()
{
    if(ui->btn_SelectedPlayerTeamTags->isChecked() )
    {
        ui->btn_SelectedPlayerTeamTags->setText("ON");
        playerInfo[SelectedPlayer].setTeamTags(true);
    }
    else
    {
        ui->btn_SelectedPlayerTeamTags->setText("OFF");
        playerInfo[SelectedPlayer].setTeamTags(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::playerButtonPressed(int value)
{
    value++;        //to silence compiler warning
    elapsedTime.start();
}

void PlayersWindow::playerButtonReleased(int value)
{
    if (elapsedTime.elapsed() > PRESS_AND_HOLD_TIME)
    {
        RenamePlayer(value);
        PlayerButtons[value]->setChecked(false);
        if(playerInfo[value].getPlayerName() == "")
        {
            if (gameInfo.getNumberOfTeams() == 0)
            {
                PlayerButtons[value]->setText("Player " + QString::number(value));
                return;
            }
            if (value > 0)  PlayerButtons[value]->setText("Player " + QString::number(value)    );
            if (value > 8)  PlayerButtons[value]->setText("Player " + QString::number(value - 8));
            if (value > 16) PlayerButtons[value]->setText("Player " + QString::number(value -16));
        }
    }
    else
    {
        setSelectedPlayer(value);

        //Reset all buttons to default stylesheet settings.
        for (int x = 1; x < 25; x++)
        {
            if (PlayerButtons[x]->isChecked() ) PlayerButtons[x]->setStyleSheet("background-color: rgb(50,220,200)");
            else                                PlayerButtons[x]->setStyleSheet("background-color: cyan");
        }
        //Set the last pressed button to a darker colour
        PlayerButtons[value]->setStyleSheet("background-color: rgb(50,250,220)");

        LoadPlayerSettings(value);
        AdjustSettingsForHandicap(value);
    }
}

void PlayersWindow::RenamePlayer(int player)
{
    QString text = QInputDialog::getText(this, tr("Rename Player"), tr("Enter Player name:"), QLineEdit::Normal, playerInfo[player].getPlayerName());
    if (text == "")
    {
        text = "Player " + QString::number(player);
        playerInfo[player].setPlayerName(text);

        if(gameInfo.getNumberOfTeams() > 1)
        {
            if(player >   8) text = "Player " + QString::number(player- 8);
            if (player > 16) text = "Player " + QString::number(player-16);

        }
    }
    PlayerButtons[player]->setText(text);

//    //Rename player buttons if they have the default name
//    if (playerInfo[player].getPlayerName() == "Player " + QString::number(player))
//    {
//        if(gameInfo.getNumberOfTeams() > 1)
//        {
//            if(player > 8)          PlayerButtons[player]->setText("Player " + QString::number(player-8) );
//            else if (player > 16)   PlayerButtons[player]->setText("Player " + QString::number(player-16));
//        }
//    }
}

/////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::AdjustSettingsForHandicap(int currentPlayer)
{
    int health      = playerInfo[currentPlayer].getHealthTags();
    int shields     = playerInfo[currentPlayer].getShieldTime();
    int megas       = playerInfo[currentPlayer].getMegaTags();
    int reloads     = playerInfo[currentPlayer].getReloads();

    if (playerInfo[currentPlayer].getHandicap() == 0)
    {
        LoadPlayerSettings(currentPlayer);
    }
    else
    {
        health  = playerInfo[currentPlayer].handicapAdjust(health);
        shields = playerInfo[currentPlayer].handicapAdjust(shields);
        megas   = playerInfo[currentPlayer].handicapAdjust(megas);
        reloads = playerInfo[currentPlayer].handicapAdjust(reloads);
    }
    ui->label_Health  ->setText("Health : "   + (QString::number(health)) + " tags");
    ui->label_Shields ->setText("Shields : "  + (QString::number(shields)) + " seconds");

    if  (megas == 100) ui->label_MegaTags->setText("MegaTags : Unlimited");
    else               ui->label_MegaTags->setText("MegaTags : " + QString::number(megas) );

    if (reloads == 100) ui->label_Reloads->setText("Reloads : Unlimited");
    else                ui->label_Reloads->setText("Reloads : " + (QString::number(reloads)) );
}

//////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::on_btn_EditMode_clicked()
{
    if (ui->btn_EditMode->isChecked() == true)
    {
       SetPlayerControls(true, EDIT_SETTINGS_MODE);
        ui->btn_EditMode->setText("Edit Handicap");

    }
    else if (ui->btn_EditMode->isChecked() == false)
    {
        SetPlayerControls(true, HANDICAP_MODE);
        ui->btn_EditMode->setText("Edit Settings");
    }
}

void PlayersWindow::on_slider_Handicap_valueChanged(int value)
{
   if (value > 0) ui->label_Handicap->setText("Handicap : +" + QString::number(value*10) + "%" );
   else           ui->label_Handicap->setText("Handicap : "  + QString::number(value*10) + "%" );
    playerInfo[SelectedPlayer].setHandicap(value);
    AdjustSettingsForHandicap(SelectedPlayer);
}

void PlayersWindow::on_slider_Reloads_valueChanged(int value)
{
    if  (value == 100) ui->label_Reloads->setText("Reloads : Unlimited");
    else ui->label_Reloads->setText("Reloads : " + QString::number(value) );
    playerInfo[SelectedPlayer].setReloads(value);
}

void PlayersWindow::on_slider_Health_valueChanged(int value)
{
     ui->label_Health->setText("Health : " + QString::number(value) + " tags");
     playerInfo[SelectedPlayer].setHealthTags(value);
}

void PlayersWindow::on_slider_Shields_valueChanged(int value)
{
     ui->label_Shields->setText("Shields : " + QString::number(value) + " seconds");
     playerInfo[SelectedPlayer].setShieldTime(value);
}

void PlayersWindow::on_slider_MegaTags_valueChanged(int value)
{
     if  (value == 100) ui->label_MegaTags->setText("MegaTags : Unlimited");
     else               ui->label_MegaTags->setText("MegaTags : " + QString::number(value) );
     playerInfo[SelectedPlayer].setMegaTags(value);
}

void PlayersWindow::on_btn_Test_clicked()
{
    qDebug() << "PlayersWindow::on_btn_Test_clicked() - Team1 PlayersByte:" <<gameInfo.getPlayersInTeam(1);
    qDebug() << "PlayersWindow::on_btn_Test_clicked() - Team2 PlayersByte:" <<gameInfo.getPlayersInTeam(2);
    qDebug() << "PlayersWindow::on_btn_Test_clicked() - Team3 PlayersByte:" << gameInfo.getPlayersInTeam(3);
}
