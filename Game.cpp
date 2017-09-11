#include "Game.h"
#include <QDebug>
#include <QString>
#include "Defines.h"
#include "Players.h"

Game    gameInfo;

Game::Game()    // (QObject *parent)
{
    //qDebug() << "Game::Game() - Constructing.......";

    GameType        = DEFAULT_GAME_TYPE;
    GameID          = DEFAULT_GAME_ID;
    GameLength      = DEFAULT_GAME_LENGTH;
    NumberOfPlayers = 24;
    GameName        ="LTTO";
    NameChar1       = 49;  // 1
    NameChar2       = 50;  // 2
    NameChar3       = 51;  // 3
    NameChar4       = 52;  // 4

    PlayerToReHost = 0;
    isSpiesTeamTagActive = false;

    CountDownTime   = DEFAULT_COUNTDOWN_TIME;
    isThisPlayerInTheGame[0] = true;        //this is required so that Announce continues after all Taggers are hosted.
    for (int x= 1; x<25; x++)
    {
        isThisPlayerInTheGame[x] = false;
    }
    for (int index = 1; index < 3; index++)
    {
        PlayersInTeamByte[index] = 0;
    }
}

int Game::getGameType() const
{
    return GameType;
}

void Game::setGameType(int value)
{
    GameType = value;

    for(int index = 0; index < 25; index++)
    {
        // Reset Game Specific Flags and buttons.
        playerInfo[index].setBitFlags1(HUNT_THE_PREY_FLAG, false);
        playerInfo[index].setBitFlags1(REVERSE_HUNT_DIR_FLAG, false);
        playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, false);
        playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG, false);

        // Set Game Specific Flags and Number of Teams Flags
        switch(value)
        {
        case Ltag0:
            setNumberOfTeams(0);
            break;
        case Ltag2:
            setNumberOfTeams(2);
            break;
        case Ltag3:
            setNumberOfTeams(3);
            break;

        case HideSeek2:
            playerInfo[index].setBitFlags1(HUNT_THE_PREY_FLAG, true);
            setNumberOfTeams(2);
            break;
        case HideSeek3:
            playerInfo[index].setBitFlags1(HUNT_THE_PREY_FLAG, true);
            setNumberOfTeams(3);
            break;

        case Kings2:
            setNumberOfTeams(2);
            break;
        case Kings3:
            setNumberOfTeams(3);
            break;

        case OwnZone0:
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG, true);
            playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, true);
            setNumberOfTeams(0);
            break;
        case OwnZone2:
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG, true);
            playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, true);
            setNumberOfTeams(2);
            break;
        case OwnZone3:
            playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, true);
            setNumberOfTeams(3);
            break;

        case Custom:
            //TODO: How to deal with different numbers of teams ????
            break;

        case LtarGame:
            //TODO: What do I do here.
            setIsLTARGame(true);

        }
    }
   // qDebug() << "\n\tFlags: "  << playerInfo[0].displayBinary(playerInfo[0].getPackedFlags1(), 8)
   //          << "\t: "          << playerInfo[0].displayBinary(playerInfo[0].getPackedFlags2(), 8);
}

int Game::getGameID() const
{
    return GameID;
}

void Game::setGameID(int value)
{
    GameID = value;
}

int Game::getGameLength() const
{
    return GameLength;
}

void Game::setGameLength(int value)
{
    GameLength = value;
}

int Game::getNumberOfPlayers() const
{
    return NumberOfPlayers;
}

void Game::setNumberOfPlayers(int value)
{
    NumberOfPlayers = value;
}

int Game::getNumberOfTeams() const
{
    return NumberOfTeams;
}

void Game::setNumberOfTeams(int value)
{
    NumberOfTeams = value;

    //now set the Flags2 Team bits
    bool msb, lsb;
    switch (value)
    {
        case 0:
            msb = 0;
            lsb = 1;
            break;
        case 1:
            msb = 0;
            lsb = 1;
            break;
        case 2:
            msb = 1;
            lsb = 0;
            break;
        case 3:
            msb = 1;
            lsb = 1;
            break;
        default:
            msb = 0;
            lsb = 0;
    }
    for(int x = 0; x < 25; x++)
    {
        playerInfo[x].setBitFlags2(TEAM_COUNT_MSB, msb);
        playerInfo[x].setBitFlags2(TEAM_COUNT_LSB, lsb);
    }
}

int Game::getNumberOfSpies() const
{
    return NumberOfSpies;
}

void Game::setNumberOfSpies(int value)
{
    NumberOfSpies = value;
}

QString Game::getGameName() const
{
    return GameName;
}

void Game::setGameName(QString value)
{
    GameName = value;
}

bool Game::getIsThisPlayerInTheGame(int index) const
{
    return isThisPlayerInTheGame[index];
}

void Game::setIsThisPlayerInTheGame(int index, int value)
{
    isThisPlayerInTheGame[index] = value;
}

int Game::getPlayersInTeam(int TeamNumber) const
{
    //TODO: Get a sum of the number of players in the team (0 thru 8)
    int numberOfPlayersInThisTeam = 0;
    int startingPlayer = 1 + (8*(TeamNumber-1));
    int endingPlayer = 1+(8*TeamNumber);
    for (int index = startingPlayer; index < endingPlayer; index++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(index) == true) numberOfPlayersInThisTeam++;
    }
    qDebug() << "Game::getPlayersInTeam() - Number of players = " << numberOfPlayersInThisTeam << "in Team" << TeamNumber;
    return numberOfPlayersInThisTeam;
}

void Game::setPlayersInTeamByte(int TeamNumber, int PlayerNumber, bool state)
{
    qDebug() << "Game::setPlayersInTeamByte() -------  NOT NEEDED, delete this call";
    //TODO: This is wrong. Just needs to be a sum of the number of players in the team.
    PlayersInTeamByte[TeamNumber] ^= (-state ^ PlayersInTeamByte[TeamNumber]) & (1 << (PlayerNumber-1));
}
////////////////////////////////////////////////////////////////////////////////////

void Game::streamToFile(QTextStream &out)
{
    out << "GameID:"           << GameID           << endl;
    out << "GameLength:"       << GameLength       << endl;
    out << "GameName:"         << GameName         << endl;
    out << "GameType:"         << GameType         << endl;
    out << "NumberOfPlayers:"  << NumberOfPlayers  << endl;
    out << "NumberOfTeams:"    << NumberOfTeams    << endl;
    out << "NumberOfSpies:"    << NumberOfSpies    << endl;
    out << "CountDownTime:"    << CountDownTime    << endl;
    out << "PlayersInGame;"    << endl;
    out << "SpyTeamTagsActive:"<< isSpiesTeamTagActive << endl;
    for (int x=0; x< 25; x++)
    {
        if      (x < 10) out << " Player0" << x << ":" << isThisPlayerInTheGame[x] << endl;
        else             out << " Player"  << x << ":" << isThisPlayerInTheGame[x] << endl;;
    }

    out << "END_OF_GAME_SETTINGS" << endl;
}


void Game::streamFromFile(QTextStream &in)
{
    QString descriptorG;

    do
    {
            descriptorG = in.readLine();
            //qDebug() << descriptorG << descriptorG.indexOf(":");
            if      (descriptorG.contains("GameID:") )              GameID                  = extractInteger(descriptorG);
            else if (descriptorG.contains("GameLength:") )          GameLength              = extractInteger(descriptorG);
            else if (descriptorG.contains("GameType:") )            GameType                = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfPlayers:") )     NumberOfPlayers         = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfTeams:") )       NumberOfTeams           = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfSpies:") )       NumberOfSpies           = extractInteger(descriptorG);
            else if (descriptorG.contains("GameName:") )            GameName                = descriptorG;
            else if (descriptorG.contains(" Player") )              isThisPlayerInTheGame[descriptorG.mid(7,2).toInt()] = descriptorG.right(1).toInt();
            else if (descriptorG.contains("CountDownTime:") )       CountDownTime           = extractInteger(descriptorG);
            else if (descriptorG.contains("SpyTeamTagsActive:") )   isSpiesTeamTagActive    = extractInteger(descriptorG);
    }   while (descriptorG != "END_OF_GAME_SETTINGS");

    setNumberOfTeams(NumberOfTeams);    //This is required to update the Flags2 bits.





    qDebug() << "GameID:"           << GameID;
    qDebug() << "GameLength:"       << GameLength;
    qDebug() << "GameName:"         << GameName;
    qDebug() << "GameType:"         << GameType;
    qDebug() << "NumberOfPlayers:"  << NumberOfPlayers;
    qDebug() << "NumberOfTeams:"    << NumberOfTeams;
    qDebug() << "NumberOfSpies:"    << NumberOfSpies;
    qDebug() << "CountDownTime"    << CountDownTime;
    qDebug() << "PlayersInGame;";
    for (int x=0; x< 25; x++)
    {
        qDebug() << "  Player"  << x << ":" << isThisPlayerInTheGame[x];
    }
    qDebug() << endl << "Game::StreamFromFile has left the building" << endl;
}

//int Game::getPlayersInGameByte() const
//{
//    return PlayersInGameByte;
//}

//void Game::setPlayersInGameByte(int value)
//{
//    PlayersInGameByte = value;
//}

int Game::getCountDownTime() const
{
    return CountDownTime;
}

void Game::setCountDownTime(int value)
{
    CountDownTime = value;
}

char Game::getNameChar1() const
{
    return NameChar1;
}

void Game::setNameChar1(char value)
{
    NameChar1 = value;
}

char Game::getNameChar2() const
{
    return NameChar2;
}

void Game::setNameChar2(char value)
{
    NameChar2 = value;
}

char Game::getNameChar3() const
{
    return NameChar3;
}

void Game::setNameChar3(char value)
{
    NameChar3 = value;
}

char Game::getNameChar4() const
{
    return NameChar4;
}

void Game::setNameChar4(char value)
{
    NameChar4 = value;
}

bool Game::getIsSpiesTeamTagActive() const
{
    return isSpiesTeamTagActive;
}

void Game::setIsSpiesTeamTagActive(bool value)
{
    isSpiesTeamTagActive = value;
}

int Game::getTotalNumberOfPlayersInGame() const
{
    int count = 0;
    for (int x = 1; x<25;x++)
    {
        if(getIsThisPlayerInTheGame(x)) count++;
    }
    return count;
}

int Game::getPlayerToReHost() const
{
    return PlayerToReHost;
}

void Game::setPlayerToReHost(int value)
{
    PlayerToReHost = value;
}

bool Game::getIsLTARGame() const
{
    return isLTARGame;
}

void Game::setIsLTARGame(bool value)
{
    isLTARGame = value;
}

int Game::extractInteger(QString &dG)
{
    return dG.right((dG.length() - (dG.indexOf(":")+1) )).toInt();      //extracts all the chars to right of the ":" and convert to an Int
}
