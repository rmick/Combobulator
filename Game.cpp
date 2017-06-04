#include "Game.h"
#include<QDebug>
#include <QString>

Game        gameInfo;

Game::Game()
{
    GameType        = 0x02;
    GameID          = 0x7C;
    GameLength      = 10;
    NumberOfPlayers = 24;
    GameName        ="LTTO";
    CountDownTime   = 5;
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

QString Game::getGameTypeTx() const
{
    QString _gameType = QString::number(GameType, 10).toUpper();
    if (_gameType.length() == 1) _gameType.prepend('0');
    return _gameType;
}

int Game::getGameID() const
{
    return GameID;
}

void Game::setGameID(int value)
{
    GameID = value;
}

QString Game::getGameIdTx() const
{
    QString _gameID = QString::number(GameID, 10).toUpper();
    if (_gameID.length() == 1) _gameID.prepend('0');
    return _gameID;
}

int Game::getGameLength() const
{
    return GameLength;
}

void Game::setGameLength(int value)
{
    GameLength = value;
}

QString Game::getGameLengthTx() const
{
    return QString::number(GameLength);
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

int Game::getPlayersInTeamTx(int TeamNumber) const
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

int Game::extractInteger(QString &dG)
{
    return dG.right((dG.length() - (dG.indexOf(":")+1) )).toInt();      //extracts all the chars to right of the ":" and convert to an Int
}

////////////////////////////////////////////////////////////////////////////////////

int calculatePlayersInGameByte()
{
    return false; //TODO: This needs to be calculated
}
