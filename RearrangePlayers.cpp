#include "RearrangePlayers.h"
#include "ui_RearrangePlayers.h"

#include <QDebug>
#include <QMessageBox>
#include <QRandomGenerator>

RearrangePlayers::RearrangePlayers(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RearrangePlayers)
{
	ui->setupUi(this);

	numberOfPlayersInEachTeam = MAX_PLAYERS/3;
    populateListWithPlayers();

	//ui->btn_SetDefaultTeams->setVisible(false);
//    ui->label_TheBench->setVisible(false);
//    ui->list_Bench->setVisible(false);
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
		QMessageBox::critical(0,"Error","There are too many players in one of the teams.\nPlease fix and try again.");
		return;
	}
	else
	{
		reAssignPlayerData();
		close();
	}

}

void RearrangePlayers::clearPlayerLists()
{
		ui->list_Team1->clear();
		ui->list_Team2->clear();
		ui->list_Team3->clear();
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
//        if(index < 5)
//        {
//            ui->list_Bench->addItem("Sub " + QString::number(index));
//            ui->list_Bench->item(index-1)->setTextAlignment(Qt::AlignCenter);
//            ui->list_Bench->item(index-1)->setData(Qt::UserRole, index+24);
//        }
	}
	reSizeListWidgetButtonWidth();
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

void RearrangePlayers::assignPlayersToTeams(bool isRandom)
{
	//TODO: Need to modify to only randomise selected players
	//		Also need to respect Team balances (eg. 5 in Team1 and 3 in Team2).
	//Pseudo code
	// Check how many players on Team1/2/3
	// Randomise only selected players
	// Check Team1/2/3 numbers are correct, if not force some to move.

	//BUG: Rearrange to default also clears Players Names !!!!!

	uint8_t	numberOfPlayersToShuffle = MAX_PLAYERS;
	if(gameInfo.getNumberOfTeams() == 2) numberOfPlayersToShuffle = 16;


	//	Create an array and initialise all as Zer0.
	int newPlayerNumber[MAX_PLAYERS+1];
	for(int index = 1; index <= numberOfPlayersToShuffle; index++)
	{
		newPlayerNumber[index] = index;
	}

	//Shuffle the array
	if(isRandom) std::random_shuffle(std::begin(newPlayerNumber)+1, std::end(newPlayerNumber));


	// Update the player numbers and refresh the grid.
	for(int index = 1; index <= numberOfPlayersToShuffle; index++)
	{
		if (isRandom)	playerInfo[0].copyPlayerSettings(index, newPlayerNumber[index] );
		else			playerInfo[0].copyPlayerSettings(index, playerInfo[index].getPlayerIndex() );
	}
	playerInfo[0].moveAllPlayersFromTempToMain();
	clearPlayerLists();
	populateListWithPlayers();
}

void RearrangePlayers::reAssignPlayerData()
{
	int playerToCopy    = 0;

	for(int index = 1; index <= MAX_PLAYERS; index++)
	{
		if      (index <= numberOfPlayersInEachTeam    )	playerToCopy = ui->list_Team1->item(index -1)->data(Qt::UserRole).toInt();
		else if (index <= numberOfPlayersInEachTeam * 2)	playerToCopy = ui->list_Team2->item(index -9)->data(Qt::UserRole).toInt();
		else if (index <= numberOfPlayersInEachTeam * 3)	playerToCopy = ui->list_Team3->item(index-17)->data(Qt::UserRole).toInt();

		playerInfo[0].copyPlayerSettings(playerToCopy, index);
	}
	playerInfo[0].moveAllPlayersFromTempToMain();

	emit dataUpdated(); //This updates the buttons in PlayerWindow.cpp

	// TODO: Clean out the PlayerInfoTemp database
	// TODO: Also reset/disable missing players with default data and set as not in the game !
}

void RearrangePlayers::on_btn_Randomise_clicked()
{
	assignPlayersToTeams(true);
}

void RearrangePlayers::on_btn_SetDefaultTeams_clicked()
{
	assignPlayersToTeams(false);
}
