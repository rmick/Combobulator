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
    QString getReloadsTx() const;		    //returns Reloads as QString as 8bit-BCD  (converts 100 -> FF = Unlimited)

    int getHealthTags() const;
    void setHealthTags(int value);
    QString  getHealthTagsTx() const;		    //returns HealthTags as QString as 8bit-BCD

    int getShieldTime() const;
    void setShieldTime(int value);
    QString getShieldTimeTx() const;                //returns HealthTags as QString as 8bit-BCD

    int getMegaTags() const;
    void setMegaTags(int value);
    QString getMegaTagsTx() const;                  //returns HealthTags as QString as 8bit-BCD (converts 100 -> FF = Unlimited)

    bool getSlowTags() const;
    void setSlowTags(bool value);

    bool getTeamTags() const;
    void setTeamTags(bool value);

    bool getMedicMode() const;
    void setMedicMode(bool value);

    void streamToFile(QTextStream &out);
    void streamFromFile(QTextStream &in);

    int  getPackedFlags1() const;
    void setPackedFlag1_NeutraliseWhenTagged(bool state);
    void setPackedFlag1_LimitedReloads(bool state);
    void setPackedFlag1_LimitedMegaTags(bool state);
    void setPackedFlag1_TeamTags(bool state);
    void setPackedFlag1_MedicMode(bool state);
    void setPackedFlag1_SlowTags(bool state);
    void setPackedFlag1_HuntThePrey(bool state);
    void setPackedFlag1_HuntDirection(bool state);
    void setPackedFlags1(int value);
    QString getPackedFlags1Tx() const;

    int getPackedFlags2() const;

    void setPackedFlags2(int value);
    QString getPackedFlags2Tx() const;

    int getTaggerID() const;
    void setTaggerID(int value);

    bool getIsInTheGame() const;
    void setIsInTheGame(bool value);

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
};

extern Players playerInfo[25];

#endif // PLAYERS_H
