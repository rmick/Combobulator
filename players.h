#ifndef PLAYERS_H
#define PLAYERS_H

#include <QString>
#include <QTextStream>
#include <Defines.h>

class Players
{
public:
    Players();

    bool testing123;

    int  getHandicap() const;
    void setHandicap(int value);

    QString getPlayerName() const;
    void    setPlayerName(const QString &value);

    int getReloads() const;
    int  getReloads(bool isLtar);
    void setReloads(int value);
    int  getReloads2() const;
    void setReloads2(int value);

    int  getHealthTags() const;
    void setHealthTags(int value);

    int  getShieldTime() const;
    void setShieldTime(int value);

    int  getMegaTags() const;
    void setMegaTags(int value);

    bool getSlowTags() const;
    void setSlowTags(bool value);

    bool getTeamTags() const;
    void setTeamTags(bool value);

    bool getMedicMode() const;
    void setMedicMode(bool value);

    void streamToFile(QTextStream &out);
    void streamFromFile(QTextStream &in);

    int  getPackedFlags1() const;
    void setPackedFlags1(int value);
    void setBitFlags1(int bitNumber, bool state);
    int  getBitFlags1(int bitNumber) const;

    int  getPackedFlags2() const;
    void setPackedFlags2(int value);;
    void setBitFlags2(int bitNumber, bool state);
    int  getBitFlags2(int bitNumber) const;

    int  getPackedFlags3() const;
    void setPackedFlags3(int value);
    void setBitFlags3(int bitNumber, bool state);
    int  getBitFlags3(int bitNumber) const;

    int  getTaggerID() const;
    void setTaggerID(int value);

//    bool getIsInTheGame() const;
//    void setIsInTheGame(bool value);

    int getSpyNumber() const;
    void setSpyNumber(int value);

    QString displayBinary(int number, int digits);

    int handicapAdjust(int value);
    int handicapAdjust(int value, int maxValue);

    int getTagsTaken(int playerNumber) const;
    void setTagsTaken(int playerNumber, int value);

    int getSurvivalTimeMinutes() const;
    void setSurvivalTimeMinutes(int value);

    int getSurvivalTimeSeconds() const;
    void setSurvivalTimeSeconds(int value);

    int getZoneTimeMinutes() const;
    void setZoneTimeMinutes(int value);

    int getZoneTimeSeconds() const;
    void setZoneTimeSeconds(int value);

    int copyTo() const;
    void setReportFlags(int value);

    int getStartingAmmo() const;
    void setStartingAmmo(int value);

    int getSleepTimeOut() const;
    void setSleepTimeOut(int value);

    int getIsKing() const;
    void setIsKing(int value);

    QString getSurvivalTimeString() const;

    QString getZoneTimeString() const;

    int getTotalTagsLanded(int thisPlayer) const;

    QString getHitsAndTags(int thisPlayer, int otherPlayer) const;

    int getRankingInGame() const;
    void setRankingInGame(int value);

    int getGameScore() const;
    void setGameScore(int value);

    int getPlayerNumberInThisGame() const;
    void setPlayerNumberInThisGame(int value);

    int getTotalTagsTaken() const;
    void setTotalTagsTaken(int value);

    void copyPlayerSettings(int copyFrom, int copyTo);

    void moveAllPlayersFromTempToMain();

private:
    // Player settings - stored in saved games
    int     Handicap;
    QString PlayerName;
    int     Reloads;
    int     Reloads2;       //LTAR games only
    int     HealthTags;
    int     ShieldTime;
    int     MegaTags;
    bool    SlowTags;
    bool    TeamTags;
    bool    MedicMode;
    int     PackedFlags1;
    int     PackedFlags2;
    int     PackedFlags3;   //LTAR games only
    int     StartingAmmo;   //LTAR games only
    int     SleepTimeOut;   //LTAR games only   0 = disabled

    //  Setting used in Hosted Games - not stored in saved games
    int     TaggerID;
    int     SpyNumber;
    int     isKing;
    int     playerNumberInThisGame;
    int     gameScore;
    int     rankingInGame;
    int     tagsTaken[25];
    int     totalTagsTaken;
    int     survivalTimeMinutes;
    int     survivalTimeSeconds;
    int     zoneTimeMinutes;
    int     zoneTimeSeconds;
    int     reportFlags;
};

extern Players playerInfo[MAX_PLAYERS+1];

extern Players playerInfoTemp[MAX_PLAYERS+1];

#endif // PLAYERS_H
