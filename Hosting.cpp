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

////////////////////////////////////////////////////////////////////////////

bool Hosting::pickTheSpies()
{
    emit AddToHostWindowListWidget("HostGameWindow::assignSpies()" + QString::number(gameInfo.getNumberOfSpies()));

    //Assign all players Spy = 0
    for (int index = 0; index < 25; index++)
    {
        playerInfo[index].setSpyNumber(0);
    }

    if(gameInfo.getNumberOfSpies() == 0) return false;

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

////////////////////////////////////////////////////////////////////////////

bool Hosting::pickTheKing()
{
    //Pick a random player from each team as the King, but make sure it is not a spy! then swap them into Player1.
    bool loop = false;
    int  kingPlayerNumber;

    qDebug() << "";
    qDebug() << "------------------------------------------------";
    for (int teamNumber = 0; teamNumber < gameInfo.getNumberOfTeams(); teamNumber++)
    {
        loop = true;
        while (loop == true)
        {
            kingPlayerNumber = GetRandomNumber( (1+(teamNumber*8)), (8+(teamNumber*8)) );
            //make sure that they are not a Spy && are in the game.
            if (playerInfo[kingPlayerNumber].getSpyNumber() == 0 && gameInfo.getIsThisPlayerInTheGame(kingPlayerNumber))
            {
                playerInfo[kingPlayerNumber].setIsKing(true);
                qDebug() << "Player " << kingPlayerNumber <<  "\tis the King" << kingPlayerNumber  << "\tand is in the game - " << gameInfo.getIsThisPlayerInTheGame(kingPlayerNumber);
                loop = false;
            }
        }
    }
    qDebug() << "------------------------------------------------";
    qDebug() << "";

    return true;
}


////////////////////////////////////////////////////////////////////////////

int Hosting::GetRandomNumber(int min, int max)
{
    return ((qrand() % ((max + 1) - min)) + min);
}

////////////////////////////////////////////////////////////////////////////

int Hosting::assignTeamsAndSwapIfSpy(int currentPlayer)
{
    int assignedTeamNumber = 0;

    if      (currentPlayer > 0  && currentPlayer < 9)           // Team 1
    {
        if (playerInfo[currentPlayer].getSpyNumber() == 0)      assignedTeamNumber = 1;     // Not a Spy
        else
        {
            if      (gameInfo.getNumberOfTeams() == 2)          assignedTeamNumber = 2;     // Increment to Team 2
            else if (gameInfo.getNumberOfTeams() == 3)          assignedTeamNumber = 2;     // Increment to Team 2
        }
    }
    else if (currentPlayer > 8  && currentPlayer < 17)          // Team 2
    {
        if (playerInfo[currentPlayer].getSpyNumber() == 0)      assignedTeamNumber = 2;     // Not a Spy
        else
        {
            if      (gameInfo.getNumberOfTeams() == 2)          assignedTeamNumber = 1;     //Decrement to Team 2
            else if (gameInfo.getNumberOfTeams() == 3)          assignedTeamNumber = 3;     //Increment to Team 3
        }
    }
    else if (currentPlayer > 16 && currentPlayer < 25)          // Team 3
    {
        if (playerInfo[currentPlayer].getSpyNumber() == 0)      assignedTeamNumber = 3;     // Not a Spy
        else
        {
            if      (gameInfo.getNumberOfTeams() == 2)          assignedTeamNumber = 1;     //Impossible to get here.
            else if (gameInfo.getNumberOfTeams() == 3)          assignedTeamNumber = 1;     //Increment/Rollover to Team 1
        }
    }
    return assignedTeamNumber;
}

////////////////////////////////////////////////////////////////////////////

int Hosting::swapSpyPlayers(int currentPlayer)
{
    int assignedPlayerNumber = 0;
    // Now the fun part, Swapping player numbers with the other spy.
        // eg,  T1-P3 and T2-P7 becomes...
        //      T2-P7 and T1-P3 WHILST keeping their own settings!!!
        //      This will make debrief interesting..........

    if      (currentPlayer >0 && currentPlayer < 9)      //Team 1
    {
        for(int n = 1; n <= gameInfo.getNumberOfSpies(); n++)
        {
            if      (playerInfo[currentPlayer].getSpyNumber() == n)
            {
                for (int x = 9; x < 17; x++)                // Swap a player from Team 2
                {
                    if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-9;
                }
            }
        }
    }
    else if (currentPlayer >8 && currentPlayer < 17)      //Team 2
    {
        for(int n = 1; n <= gameInfo.getNumberOfSpies(); n++)
        if      (playerInfo[currentPlayer].getSpyNumber() == n)
        {
            if(gameInfo.getNumberOfTeams() == 2)
            {
                for (int x = 1; x < 9; x++)             // Swap a player from Team 1
                {
                    if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-1;
                }
            }
            else
            {
                for (int x = 17; x < 25; x++)           // Swap a player from Team 3
                {
                    if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-17;
                }
            }
        }
    }
    else if (currentPlayer >16 && currentPlayer < 25)      //Team 3
    {
        for(int n = 1; n <= gameInfo.getNumberOfSpies(); n++)
        if      (playerInfo[currentPlayer].getSpyNumber() == n)
        {
            for (int x = 1; x < 9; x++)             // Swap a player from Team 1
            {
                if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-1;
            }
        }
    }
    return assignedPlayerNumber;
}

////////////////////////////////////////////////////////////////////////////

int Hosting::swapKingPlayers(int currentPlayer)
{
    int assignedPlayerNumber = 0;
    bool theKing = playerInfo[currentPlayer].getIsKing();

    qDebug() << "Hosting::swapKingPlayers() - is a King ?" << theKing;

    //  Psuedo code
    //  If Player1 && King - do nothing
    //  If !Player1 && !King - also do nothing.
    //  If !Player1 && King - swap with Player 1
    //  If Player1 && !King - swap with the King

    // If player is already player1 - do nothing and exit
    if(currentPlayer == 1  && theKing)  return assignedPlayerNumber;
    if(currentPlayer == 9  && theKing)  return assignedPlayerNumber;
    if(currentPlayer == 17 && theKing)  return assignedPlayerNumber;

    //
    if      (currentPlayer >0 && currentPlayer < 9)      //Team 1
    {
        if(theKing)
        {
            assignedPlayerNumber = 0;
        }
        else if(currentPlayer == 1)
        {
            for (int index = 1; index < 9; index++)
            {
                if(playerInfo[index].getIsKing()) assignedPlayerNumber = index - 1;
            }
        }
        else assignedPlayerNumber = currentPlayer -1;

    }
    else if (currentPlayer >8 && currentPlayer < 17)      //Team 2
    {
        if(theKing)
        {
            assignedPlayerNumber = 0;
        }
        else if(currentPlayer == 9)
        {
            for (int index = 9; index < 17; index++)
            {
                if(playerInfo[index].getIsKing()) assignedPlayerNumber = index - 9;
            }
        }
        else assignedPlayerNumber = currentPlayer -9;
    }
    else if (currentPlayer >16 && currentPlayer < 25)      //Team 3
    {
        if(theKing)
        {
            assignedPlayerNumber = 0;
        }
        else if(currentPlayer == 17)
        {
            for (int index = 17; index < 25; index++)
            {
                if(playerInfo[index].getIsKing()) assignedPlayerNumber = index - 17;
            }
        }
        else assignedPlayerNumber = currentPlayer -17;
    }

    return assignedPlayerNumber;
}
