#ifndef PLAYERS_H
#define PLAYERS_H

#include <QString>
#include <QTextStream>

class Players
{
public:
    Players();

    int getHandicap() const;
    void setHandicap(int value);

    QString getPlayerName() const;
    void setPlayerName(const QString &value);

    int getReloads() const;
    void setReloads(int value);

    int getHealthTags() const;
    void setHealthTags(int value);

    int getShieldTime() const;
    void setShieldTime(int value);

    int getMegaTags() const;
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

    int getPackedFlags2() const;
    void setPackedFlags2(int value);;
    void setBitFlags2(int bitNumber, bool state);

    int getTaggerID() const;
    void setTaggerID(int value);

    bool getIsInTheGame() const;
    void setIsInTheGame(bool value);

    int getSpyNumber() const;
    void setSpyNumber(int value);

    QString displayBinary(int number, int digits);

    int handicapAdjust(int value);

private:
    int     Handicap;
    QString PlayerName;
    int     Reloads;
    int     HealthTags;
    int     ShieldTime;
    int     MegaTags;
    bool    SlowTags;
    bool    TeamTags;
    bool    MedicMode;
    int     PackedFlags1;
    int     PackedFlags2;
    int     TaggerID;
    int     SpyNumber;
};

extern Players playerInfo[25];

#endif // PLAYERS_H
