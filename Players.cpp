#include "Players.h"
#include <QDebug>
#include "Defines.h"

Players playerInfo[25];

Players::Players()
{
        Handicap        = 0;
        PlayerName       = "_";
        Reloads         = DEFAULT_RELOADS;
        HealthTags      = DEFAULT_HEALTH;
        ShieldTime      = DEFAULT_SHEILDS;
        MegaTags        = DEFAULT_MEGAS;
        SlowTags        = false;
        TeamTags        = false;
        MedicMode       = false;
        PackedFlags1    = DEFAULT_FLAGS1;
        PackedFlags2    = DEFAULT_FLAGS2;
}

int Players::getHandicap() const
{
    return Handicap;
}

void Players::setHandicap(int value)
{
    Handicap = value;
}

QString Players::getPlayerName() const
{
    return PlayerName;
}

void Players::setPlayerName(const QString &value)
{
    PlayerName = value;
}

int Players::getReloads() const
{
    return Reloads;
}

void Players::setReloads(int value)
{
    Reloads = value;
    if (Reloads == 100) setBitFlags1(LIMITED_RELOADS_FLAG, false);
    else                setBitFlags1(LIMITED_RELOADS_FLAG, true);
}

int Players::getHealthTags() const
{
    return HealthTags;
}

void Players::setHealthTags(int value)
{
    HealthTags = value;
}

int Players::getShieldTime() const
{
    return ShieldTime;
}

void Players::setShieldTime(int value)
{
    ShieldTime = value;
}

int Players::getMegaTags() const
{
    return MegaTags;
}

void Players::setMegaTags(int value)
{
    MegaTags = value;
    if (MegaTags == 100) setBitFlags1(LIMITED_MEGAS_FLAG, false);
    else                 setBitFlags1(LIMITED_MEGAS_FLAG, true);
}

bool Players::getSlowTags() const
{
    return SlowTags;
}

void Players::setSlowTags(bool value)
{
    SlowTags = value;
    setBitFlags1(SLOW_TAG_FLAG, value);
}

bool Players::getTeamTags() const
{
    return TeamTags;
}

void Players::setTeamTags(bool value)
{
    TeamTags = value;
    setBitFlags1(TEAM_TAGS_FLAG, value);
}

bool Players::getMedicMode() const
{
    return MedicMode;
}

void Players::setMedicMode(bool value)
{
    MedicMode = value;
    setBitFlags1(MEDIC_MODE_FLAG, value);
    //setBitFlags1(NEUTRALISE_10_FLAG, value);
}

void Players::streamToFile(QTextStream &out)
{
    out << "------------------------" << endl;
    for (int x = 0; x<25; x++)
    {
        out << "PlayerID:" << x << endl;
        out << "Handicap: "     << playerInfo[x].Handicap << endl;
        out << "HealthTags: "   << playerInfo[x].HealthTags << endl;
        out << "MedicMode: "    << playerInfo[x].MedicMode << endl;
        out << "MegaTags: "     << playerInfo[x].MegaTags << endl;
        out << "PlayerName: "   << playerInfo[x].PlayerName << endl;
        out << "Reloads: "      << playerInfo[x].Reloads << endl;
        out << "ShieldTime: "   << playerInfo[x].ShieldTime << endl;
        out << "SlowTags: "     << playerInfo[x].SlowTags << endl;
        out << "TeamTags: "     << playerInfo[x].TeamTags << endl;
        out << "------------------------" << endl;
    }
    out << "END_OF_PLAYER_SETTINGS" << endl;
}


void Players::streamFromFile(QTextStream &in)
{
    QString descriptorP = NULL ;
    int playerID = NULL;

    do
    {
            descriptorP = in.readLine();
            if      (descriptorP.contains("PlayerID:") )        playerID                         = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Handicap:") )        playerInfo[playerID].Handicap    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("HealthTags:") )      playerInfo[playerID].HealthTags  = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("MegaTags:") )        playerInfo[playerID].MegaTags    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("ShieldTime:") )      playerInfo[playerID].ShieldTime  = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("MedicMode:") )       playerInfo[playerID].MedicMode   = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("SlowTags:") )        playerInfo[playerID].SlowTags    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("TeamTags:") )        playerInfo[playerID].TeamTags    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("PlayerName:") )      playerInfo[playerID].PlayerName  = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) ));
            else if (descriptorP.contains("Reloads:") )         playerInfo[playerID].Reloads     = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
    }   while (descriptorP != "END_OF_PLAYER_SETTINGS");

    for (int x=0; x< 25; x++)
    {
        qDebug() << "PlayerID:"     << x;
        qDebug() << "Handicap:"     << playerInfo[x].Handicap;
        qDebug() << "HealthTags:"   << playerInfo[x].HealthTags;
        qDebug() << "MegaTags:"     << playerInfo[x].MegaTags;
        qDebug() << "Reloads:"      << playerInfo[x].Reloads;
        qDebug() << "ShieldTime:"   << playerInfo[x].ShieldTime;
        qDebug() << "MedicMode:"    << playerInfo[x].MedicMode;
        qDebug() << "SlowTags:"     << playerInfo[x].SlowTags;
        qDebug() << "TeamTags:"     << playerInfo[x].TeamTags;
        qDebug() << "PlayerName:"   << playerInfo[x].PlayerName << endl;
    }
    qDebug() << "Players::StreamFromFile has left the building" << endl << endl;
}

int Players::getPackedFlags1() const
{
    //qDebug() << "Players::getPackedFlags1() " + QString::number(PackedFlags1, 2);

    return PackedFlags1;
}

void Players::setBitFlags1(int bitNumber, bool state)
{
    PackedFlags1 ^= (-state ^ PackedFlags1) & (1 << bitNumber);
    //qDebug() << "\tFlags1: " << getPackedFlags1() << "\t\tBinary = " << displayBinary(PackedFlags1, 8);
}

void Players::setPackedFlags1(int value)
{
    PackedFlags1 = value;
}

int Players::getPackedFlags2() const
{
    return PackedFlags2;
}

void Players::setPackedFlags2(int value)
{
    PackedFlags2 = value;
}

void Players::setBitFlags2(int bitNumber, bool state)
{
    PackedFlags2 ^= (-state ^ PackedFlags2) & (1 << bitNumber);
    //qDebug() << "\tFlags2: " << getPackedFlags2() << "\t\tBinary = " << displayBinary(PackedFlags2, 8);
}

int Players::getTaggerID() const
{
    return TaggerID;
}

void Players::setTaggerID(int value)
{
    TaggerID = value;
}

int Players::getSpyNumber() const
{
    return SpyNumber;
}

void Players::setSpyNumber(int value)
{
    SpyNumber = value;
}

QString Players::displayBinary(int number, int digits)
{
    QString output = QString::number(number, 2);
    int padding = digits - output.length();
    while (padding > 0)
    {
        output.prepend('0');
        padding--;
    }
    return output;
}

int Players::handicapAdjust(int value)
{
    value += value * (static_cast<float>(Handicap)/10);
    if (value > 100) value = 100;
    if (value <0)   value = 0;

    return value;
}
