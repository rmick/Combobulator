#include "Game.h"
#include <QDebug>
#include <QString>
#include "Defines.h"
#include "Players.h"

Game    gameInfo;

Game::Game()    // (QObject *parent)
{
    GameType        = DEFAULT_GAME_TYPE;
    GameID          = DEFAULT_GAME_ID;
    GameLength      = DEFAULT_GAME_LENGTH;
    NumberOfPlayers = 24;
    GameName        ="LTTO";
    NameChar1       = 49;  // 1
    NameChar2       = 50;  // 2
    NameChar3       = 51;  // 3
    NameChar4       = 52;  // 4

    isSpiesTeamTagActive = true;

    CountDownTime   = DEFAULT_COUNTDOWN_TIME;
    isThisPlayerInTheGame[0] = true;        //this is required so that Announce continues after all Taggers are hosted.
    for (int x= 1; x<25; x++)
    {
        isThisPlayerInTheGame[x] = false;
    }
}

int Game::getGameType() const
{
    return GameType;
}

void Game::setGameType(int value)
{
    GameType = value;
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
    //if (NumberOfTeams == 0) emit NumberOfTeamsHasChanged(NumberOfTeams);
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
    int PlayersPackedByte = 0;
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(8*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(7*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(6*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(5*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(4*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(3*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(2*TeamNumber)];
    PlayersPackedByte = PlayersPackedByte << isThisPlayerInTheGame[(1*TeamNumber)];
    qDebug() << "Game::getPlayersInTeamTx() = " << PlayersPackedByte;
    return PlayersPackedByte;
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
    out << "CountDownTime:"   << CountDownTime    << endl;
    out << "PlayersInGame;"    << endl;
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
            if      (descriptorG.contains("GameID:") )           GameID          = extractInteger(descriptorG);
            else if (descriptorG.contains("GameLength:") )       GameLength      = extractInteger(descriptorG);
            else if (descriptorG.contains("GameType:") )         GameType        = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfPlayers:") )  NumberOfPlayers = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfTeams:") )    NumberOfTeams   = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfSpies:") )    NumberOfSpies   = extractInteger(descriptorG);
            else if (descriptorG.contains("GameName:") )         GameName        = extractInteger(descriptorG);
            else if (descriptorG.contains(" Player") )           isThisPlayerInTheGame[descriptorG.mid(7,2).toInt()] = descriptorG.right(1).toInt();
            else if (descriptorG.contains("CountDownTime:") )    CountDownTime   = extractInteger(descriptorG);
    }   while (descriptorG != "END_OF_GAME_SETTINGS");

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

int Game::getPlayersInGameByte() const
{
    return PlayersInGameByte;
}

void Game::setPlayersInGameByte(int value)
{
    PlayersInGameByte = value;
}

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

int Game::extractInteger(QString &dG)
{
    return dG.right((dG.length() - (dG.indexOf(":")+1) )).toInt();      //extracts all the chars to right of the ":" and convert to an Int
}

////////////////////////////////////////////////////////////////////////////////////

int calculatePlayersInGameByte()
{
    return false; //TODO: This needs to be calculated
}
