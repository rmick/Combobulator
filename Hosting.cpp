#include "Hosting.h"
#include <QDebug>
#include <QMessageBox>
#include <QTime>

#include "HostGameWindow.h"

Hosting host;

Hosting::Hosting(QObject *parent) : QObject(parent)
{
    qDebug() << "Hosting::Hosting() - Constructing.......";
    qsrand(static_cast<uint>(QTime::currentTime().msec()));
}

bool Hosting::pickTheKing()
{
    //Pick a random player from each team as the King, but make sure it is not a spy! thenswap them into Player1.
    bool loop = false;
    int  kingPlayerNumber;

    qDebug() << "";
    qDebug() << "------------------------------------------------";
    //for (int spyNumber = 1; spyNumber <= gameInfo.getNumberOfSpies(); spyNumber++)
    //{
    for (int teamNumber = 0; teamNumber < gameInfo.getNumberOfTeams(); teamNumber++)
    {
        loop = true;
        while (loop == true)
        {
            kingPlayerNumber = GetRandomNumber( (1+(teamNumber*8)), (8+(teamNumber*8)) );
            if (playerInfo[kingPlayerNumber].getSpyNumber() == 0 && gameInfo.getIsThisPlayerInTheGame(kingPlayerNumber))
            {
                playerInfo[kingPlayerNumber].setIsKing(true);
                qDebug() << "Player " << kingPlayerNumber <<  "\tis the King" << kingPlayerNumber  << "\tand is in the game - " << gameInfo.getIsThisPlayerInTheGame(kingPlayerNumber);
                loop = false;
            }
        }
    }
    qDebug() << "------------------------------------------------";
    //}
    qDebug() << "";

    return true;
}

bool Hosting::assignSpies()
{
    emit AddToHostWindowListWidget("HostGameWindow::assignSpies()" + QString::number(gameInfo.getNumberOfSpies()));

    //Assign all players Spy = 0
    for (int x = 0; x < 25; x++)
    {
        playerInfo[x].setSpyNumber(0);
    }

    if(gameInfo.getNumberOfSpies() == 0) return true;

    // ///////////////////////////////////////////////////////////////////////////
    //add some checking so that NumberOfSpies is !> number of players in any team.
    int numberOfPlayersInTeam1 = 0;
    int numberOfPlayersInTeam2 = 0;
    int numberOfPlayersInTeam3 = 0;

    //count the number of players in each team
    for (int x = 1; x < 9; x++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(x))       numberOfPlayersInTeam1++;
        if (gameInfo.getIsThisPlayerInTheGame(x+8))     numberOfPlayersInTeam2++;
        if (gameInfo.getIsThisPlayerInTheGame(x+16))    numberOfPlayersInTeam3++;
        if (gameInfo.getNumberOfTeams() < 3) numberOfPlayersInTeam3 = 8;  // Set to 8 so that we dont get a false positive in the checks below.
    }
    //exit if the number of spies is greater than number of players
    if(    (gameInfo.getNumberOfSpies() > numberOfPlayersInTeam1) ||
           (gameInfo.getNumberOfSpies() > numberOfPlayersInTeam2) ||
           (gameInfo.getNumberOfSpies() > numberOfPlayersInTeam3)  )
    {
        QMessageBox::critical(0,"Error","There are more spies than players in one or more of your teams.\n\nThis is illogical.\n\nPlease try again.");
        return false;
    }

    //issue warning if 50% or more of the players are spies
    if(     ( (gameInfo.getNumberOfSpies()*2) >= numberOfPlayersInTeam1) ||
            ( (gameInfo.getNumberOfSpies()*2) >= numberOfPlayersInTeam2) ||
            ( (gameInfo.getNumberOfSpies()*2) >= numberOfPlayersInTeam3)  )
            {
                int action = QMessageBox::question(0,tr("Are you sure?"), tr("More than 50% of your players in one team are spies.\nAre you sure this is what you want? \n\nPress OK to continue, or Cancel to set Spies to Zero."), QMessageBox::Ok | QMessageBox::Cancel);
                if (action == QMessageBox::Cancel) gameInfo.setNumberOfSpies(0);
            }

    // ////////////////////////////////////////////////////////
    //Pick a random player from each team as a Spy and assign a spy number (Spy = 1, Spy = 2)
    int spyPlayerNumber;
    bool loop = false;

    qDebug() << "";
    qDebug() << "------------------------------------------------";
    for (int spyNumber = 1; spyNumber <= gameInfo.getNumberOfSpies(); spyNumber++)
    {
        for (int teamNumber = 0; teamNumber < gameInfo.getNumberOfTeams(); teamNumber++)
        {
            loop = true;
            while (loop == true)
            {
                spyPlayerNumber = GetRandomNumber( (1+(teamNumber*8)), (8+(teamNumber*8)) );
                if (playerInfo[spyPlayerNumber].getSpyNumber() == 0 && gameInfo.getIsThisPlayerInTheGame(spyPlayerNumber))
                {
                    playerInfo[spyPlayerNumber].setSpyNumber(spyNumber);
                    qDebug() << "Player " << spyPlayerNumber <<  "\tis Spy #" << spyNumber  << "\tand is in the game - " << gameInfo.getIsThisPlayerInTheGame(spyPlayerNumber);
                    loop = false;
                }
            }
        }
        qDebug() << "------------------------------------------------";
    }
    qDebug() << "";

    return true;
}


int Hosting::GetRandomNumber(int min, int max)
{
    return ((qrand() % ((max + 1) - min)) + min);
}
