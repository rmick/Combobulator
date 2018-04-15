#include "RearrangePlayers.h"
#include "ui_RearrangePlayers.h"

#include <QDebug>
#include <QMessageBox>

RearrangePlayers::RearrangePlayers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RearrangePlayers)
{
    ui->setupUi(this);

    numberOfPlayersInEachTeam = MAX_PLAYERS/3;
    populateListWithPlayers();
    reSizeListWidgetButtonWidth();
}

RearrangePlayers::~RearrangePlayers()
{
    delete ui;
}

void RearrangePlayers::resizeEvent(QResizeEvent*)
{
    reSizeListWidgetButtonWidth();
}


void RearrangePlayers::on_btn_Close_clicked()
{
    //Check for too many players in any team
    bool tooManyPlayers = false;
    if(ui->list_Team1->count() > numberOfPlayersInEachTeam) tooManyPlayers = true;
    if(ui->list_Team2->count() > numberOfPlayersInEachTeam) tooManyPlayers = true;
    if(ui->list_Team3->count() > numberOfPlayersInEachTeam) tooManyPlayers = true;
    if(tooManyPlayers)
    {
        //QMessageBox("Error", "There are too many players in one of the teams. Please fix and try again.",)
        QMessageBox::critical(0,"Error","There are too many players in one of the teams.\nPlease fix and try again.");
        return;
    }
    else
    {
        reAssignPlayerData();
        close();
    }

}

void RearrangePlayers::populateListWithPlayers()
{
    for(int index = 1;index < (numberOfPlayersInEachTeam+1); index++)
    {
        ui->list_Team1->addItem(playerInfo[index].getPlayerName());
        ui->list_Team1->item(index-1)->setTextAlignment(Qt::AlignCenter);
        ui->list_Team1->item(index-1)->setData(Qt::UserRole, index);
        ui->list_Team2->addItem(playerInfo[index+8].getPlayerName());
        ui->list_Team2->item(index-1)->setTextAlignment(Qt::AlignCenter);
        ui->list_Team2->item(index-1)->setData(Qt::UserRole, index+8);
        ui->list_Team3->addItem(playerInfo[index+16].getPlayerName());
        ui->list_Team3->item(index-1)->setTextAlignment(Qt::AlignCenter);
        ui->list_Team3->item(index-1)->setData(Qt::UserRole, index+16);
        if(index < 5)
        {
            ui->list_Bench->addItem("Sub " + QString::number(index));
            ui->list_Bench->item(index-1)->setTextAlignment(Qt::AlignCenter);
            ui->list_Bench->item(index-1)->setData(Qt::UserRole, index+24);
        }
    }
}

void RearrangePlayers::reSizeListWidgetButtonWidth()
{
    int minSize = (ui->list_Bench->width() / numberOfPlayersInEachTeam ) - 11;
    //reduce the size slightly so that you can a bit of any excess players in a team.
    minSize--;

    for(int index = 0; index < ui->list_Team1->count(); index++)
    {
        ui->list_Team1->item(index)->setSizeHint(QSize(minSize, ui->list_Team1->sizeHint().height() ));
    }
    for(int index = 0; index < ui->list_Team2->count(); index++)
    {
        ui->list_Team2->item(index)->setSizeHint(QSize(minSize, ui->list_Team2->sizeHint().height() ));
    }
    for(int index = 0; index < ui->list_Team3->count(); index++)
    {
        ui->list_Team3->item(index)->setSizeHint(QSize(minSize, ui->list_Team3->sizeHint().height() ));
    }
    for(int index = 0; index < ui->list_Bench->count(); index++)
    {
        ui->list_Bench->item(index)->setSizeHint(QSize(minSize, ui->list_Bench->sizeHint().height() ));
    }
}

void RearrangePlayers::reAssignPlayerData()
{
    int playerToCopy    = 0;

    for(int index = 1; index <= MAX_PLAYERS; index++)
    {
        if      (index <= numberOfPlayersInEachTeam)
        {
            playerToCopy = ui->list_Team1->item(index-1)->data(Qt::UserRole).toInt();
            qDebug() << "Team1: Copying Player" << playerToCopy << ui->list_Team1->item(index-1)->text() << "to Player" << index ;
        }
        else if (index <= numberOfPlayersInEachTeam * 2 )
        {
            qDebug() << "Team2 : Index -" << index;
            playerToCopy = ui->list_Team2->item(index-9)->data(Qt::UserRole).toInt();
            qDebug() << "Team2: Copying Player" << playerToCopy << ui->list_Team1->item(index-9)->text() << "to Player" << index ;
        }
        else if (index <= numberOfPlayersInEachTeam * 3)
        {
            playerToCopy = ui->list_Team3->item(index-17)->data(Qt::UserRole).toInt();
            qDebug() << "Team3: Copying Player" << playerToCopy << ui->list_Team1->item(index-17)->text() << "to Player" << index ;
        }

        playerInfo[0].copyPlayerSettings(playerToCopy, index);
    }
    qDebug() << "RearrangePlayers::reAssignPlayerData() - MOVING DATA BACK TO PLAYERINFO[]";
    playerInfo[0].moveAllPlayersFromTempToMain();

    emit dataUpdated();


    // TODO: Clean out the PlayerInfoTemp database
    // TODO: Also reset/disable missing players with default data and set as not in the game !
}

void RearrangePlayers::on_btn_DeBug_clicked()
{
    qDebug() << " Debug button";
    reSizeListWidgetButtonWidth();
    qDebug() << ui->list_Bench->width();

    for(int index = 0; index < ui->list_Bench->count(); index++)
    {
         qDebug() << ui->list_Bench->item(index)->text();
    }
}
