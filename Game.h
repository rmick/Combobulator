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
        Special     = 0x0C
    };

    int getGameType() const;
    void setGameType(int value);
    QString getGameTypeTx() const;              //returns GameType as QString in Hex
    //void setGameTypeRx(QString value);          //accepts QString Hex and converts to Int

    int getGameID() const;
    void setGameID(int value);
    QString getGameIdTx() const;                //returns 8bit GameID as QString

    int getGameLength() const;
    void setGameLength(int value);
    QString getGameLengthTx() const;            //returns GameLength as QString in BCD
    //void setGameLengthRx(QString value);        //accepts QString BCD and converts as Dec Int

    QString getGameName() const;
    void setGameName(QString value);

    bool getIsThisPlayerInTheGame(int index) const;
    void setIsThisPlayerInTheGame(int index, int value);
    int getPlayersInTeamTx(int TeamNumber) const;            //returns PlayersInTeam1 as a PackedByte - used in CountDown
    //int getPlayersInTeam2Tx() const;            //returns PlayersInTeam2 as a PackedByte - used in CountDown
    //int getPlayersInTeam3Tx() const;            //returns PlayersInTeam3 as a PackedByte - used in CountDown

    int getNumberOfTeams() const;
    void setNumberOfTeams(int value);

    int getNumberOfPlayers() const;
    void setNumberOfPlayers(int value);

    int getNumberOfSpies() const;
    void setNumberOfSpies(int value);

    void streamToFile(QTextStream &out);
    void streamFromFile(QTextStream &in);

    int getPlayersInGameByte() const;
    void setPlayersInGameByte(int value);

    int getCountDownTime() const;
    void setCountDownTime(int value);

private:
    int     GameType;
    int     GameID;
    int     GameLength;
    int     NumberOfPlayers;
    int     NumberOfTeams;
    QString GameName;
    bool    isThisPlayerInTheGame[25];
    int     NumberOfSpies;
    int     PlayersInGameByte;
    int     CountDownTime;

    int     extractInteger(QString &dG);
};

extern Game gameInfo;

#endif // GAME_H
