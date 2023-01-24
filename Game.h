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
        Custom      = 0x0C,
        LtarGame    = 0x81
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

    int getCountDownTime() const;
    void setCountDownTime(int value);

    char getNameChar1() const;
    void setNameChar1(char value);

    char getNameChar2() const;
    void setNameChar2(char value);

    char getNameChar3() const;
    void setNameChar3(char value);

    char    getNameChar4() const;
    void    setNameChar4(char value);

    bool    getIsSpiesTeamTagActive() const;
    void    setIsSpiesTeamTagActive(bool value);

    int     getTotalNumberOfPlayersInGame() const;

    int     getPlayerToReHost() const;
    void    setPlayerToReHost(int value);

    bool    getIsLTARGame() const;
    void    setIsLTARGame(bool value);

    bool getIsReSpawnGame() const;
    void setIsReSpawnGame(bool value);

    int getNumberOfPlayersInGame();

    int getPointsPerTagLanded() const;
    void setPointsPerTagLanded(int value);

    int getPointsPerTagLandedNegative() const;
    void setPointsPerTagLandedNegative(int value);

    int getPointsPerTagTaken() const;
    void setPointsPerTagTaken(int value);

    int getPointsPerKingHit() const;
    void setPointsPerKingHit(int value);

    int getPointsPerKingHitNegative() const;
    void setPointsPerKingHitNegative(int value);

    int getPointsPerSurvivalMinute() const;
    void setPointsPerSurvivalMinute(int value);

    int getPointsPerZoneMinute() const;
    void setPointsPerZoneMinute(int value);

    int getTeam1rank() const;
    void setTeam1rank(int value);

    int getTeam2rank() const;
    void setTeam2rank(int value);

    int getTeam3rank() const;
    void setTeam3rank(int value);

	bool getIsIndoorViewMode() const;
	void setIsIndoorViewMode(bool value);

	bool getCumulativeScoreMode() const;
	void setCumulativeScoreMode(bool value);

	bool getPowerSaveMode() const;
	void setPowerSaveMode(bool value);

	int getFontSize();
	void setFontSize(int value);

	QString getCurrentGameFileName() const;
	void setCurrentGameFileName(const QString &value);

	int getScoreHeaderFontSize() const;
	void setScoreHeaderFontSize(int value);

	int getScoreTableFontSize() const;
	void setScoreTableFontSize(int value);

    bool getIsSuperSimpleMode() const;
    void setIsSuperSimpleMode(bool value);

signals:
    void    NumberOfTeamsHasChanged(int NumTeams);

private:
    bool	powerSaveMode;
	int		fontSize;
	int		scoreHeaderFontSize;
	int		scoreTableFontSize;
	QString	currentGameFileName;

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
    int     numberOfPlayersInGame;
    int     PlayersInTeamByte[3];
    int     PlayerToReHost;
    bool    isLTARGame;
    bool    isReSpawnGame;
	bool	cumulativeScoreMode = false;
    bool	isIndoorViewMode;           //not saved with game. It is here so that it is public to other classes.
    bool    superSimpleMode = false;    //Gert Mode

    int     pointsPerTagLanded;
    int     pointsPerTagLandedNegative;
    int     pointsPerTagTaken;
    int     pointsPerKingHit;
    int     pointsPerKingHitNegative;
    int     pointsPerSurvivalMinute;
    int     pointsPerZoneMinute;

    int     team1rank;
    int     team2rank;
    int     team3rank;

    int     extractInteger(QString &dG);
};

extern	Game	gameInfo;

extern	Game	gameInfoTemp;

#endif // GAME_H
