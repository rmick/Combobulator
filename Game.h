#ifndef GAME_H
#define GAME_H

#include <QString>
#include <QTextStream>

class Game
{

public:
    Game();

    enum GameCode
    {
        Ltag0       = 0x02,
        Ltag2       = 0x03,
        Ltag3       = 0x04,
        HideSeek2   = 0x05,
        HideSeek3   = 0x06,
        Kings2      = 0x07,
        Kings3      = 0x08,
        OwnZone0    = 0x09,
        OwnZone2    = 0x0A,
        OwnZone3    = 0x0B,
        Custom      = 0x0C
    };

    int getGameType() const;
    void setGameType(int value);

    int getGameID() const;
    void setGameID(int value);

    int getGameLength() const;
    void setGameLength(int value);

    QString getGameName() const;
    void setGameName(QString value);

    bool getIsThisPlayerInTheGame(int index) const;
    void setIsThisPlayerInTheGame(int index, int value);

    int getPlayersInTeam(int TeamNumber) const;             //returns PlayersInTeam1 as a PackedByte - used in CountDown
    void setPlayersInTeamByte(int TeamNumber, int PlayerNumber, bool state);

    int getNumberOfTeams() const;
    void setNumberOfTeams(int value);

    int getNumberOfPlayers() const;
    void setNumberOfPlayers(int value);

    int getNumberOfSpies() const;
    void setNumberOfSpies(int value);

    void streamToFile(QTextStream &out);
    void streamFromFile(QTextStream &in);

//    int getPlayersInGameByte() const;
//    void setPlayersInGameByte(int value);

    int getCountDownTime() const;
    void setCountDownTime(int value);

    char getNameChar1() const;
    void setNameChar1(char value);

    char getNameChar2() const;
    void setNameChar2(char value);

    char getNameChar3() const;
    void setNameChar3(char value);

    char getNameChar4() const;
    void setNameChar4(char value);

    bool getIsSpiesTeamTagActive() const;
    void setIsSpiesTeamTagActive(bool value);

    int getTotalNumberOfPlayersInGame() const;

    int getPlayerToReHost() const;
    void setPlayerToReHost(int value);

signals:
    void NumberOfTeamsHasChanged(int NumTeams);

private:
    int     GameType;
    int     GameID;
    int     GameLength;
    int     NumberOfPlayers;
    int     NumberOfTeams;
    QString GameName;
    bool    isThisPlayerInTheGame[25];
    int     NumberOfSpies;
    int     CountDownTime;
    char    NameChar1;
    char    NameChar2;
    char    NameChar3;
    char    NameChar4;
    bool    isSpiesTeamTagActive;
    int     TotalNumberOfPlayersInGame;
    int     PlayersInTeamByte[3];
    int     PlayerToReHost;

    int     extractInteger(QString &dG);
};

extern Game gameInfo;

#endif // GAME_H
