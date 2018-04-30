#include "Players.h"
#include <QDebug>
#include "Defines.h"

Players playerInfo[MAX_PLAYERS+1];

Players playerInfoTemp[MAX_PLAYERS+1];

Players::Players()
{
    static int instanceCount = 0;
    instanceCount++;

    if(instanceCount == 1)  qDebug() << "Players::Players() - Constructing.......";

    Handicap        = 0;
    PlayerName       = "_";
    Reloads         = DEFAULT_RELOADS;
    Reloads2        = DEFAULT_RELOADS2;
    HealthTags      = DEFAULT_HEALTH;
    ShieldTime      = DEFAULT_SHEILDS;
    MegaTags        = DEFAULT_MEGAS;
    SlowTags        = false;
    TeamTags        = false;
    MedicMode       = false;
    PackedFlags1    = DEFAULT_FLAGS1;
    PackedFlags2    = DEFAULT_FLAGS2;
    PackedFlags3    = DEFAULT_FLAGS3;
    StartingAmmo    = DEFAULT_START_AMMO;
    SleepTimeOut    = DEFAULT_SLEEP_TIME_OUT;
    survivalTimeMinutes = 0;
    survivalTimeSeconds = 0;
    zoneTimeMinutes     = 0;
    zoneTimeSeconds     = 0;
    rankingInGame       = 0;
    totalTagsTaken      = 0;

    for (int index = 0; index < 25; index++)
    {
        setTagsTaken(index, 0);
    }
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

int Players::getReloads(bool isLtar)
{
    int calcReloads = 0;

    if (Reloads == 100)
    {
        calcReloads = 0xFF;
        Reloads2 = 0xFF;
    }
    else if (isLtar == true)
    {
        calcReloads = Reloads * 10;
        Reloads2 = calcReloads / 256;
        calcReloads  = calcReloads % 256;
    }
    else
    {
        calcReloads = Reloads;
    }
    return calcReloads;
}

void Players::setReloads(int value)
{
    Reloads = value;
    if (Reloads == 100)
    {
        setBitFlags1(LIMITED_RELOADS_FLAG, false);
    }
    else
    {
        setBitFlags1(LIMITED_RELOADS_FLAG, true);
    }
    return;
}

int Players::getReloads2() const
{
    return Reloads2;
}

void Players::setReloads2(int value)
{
    //This method is never called. Reloads2 is set by Players::getReloads(int, bool)
    Reloads2 = value;
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

int Players::getStartingAmmo() const
{
    if (StartingAmmo == 100) return 0xFF;
    else                    return StartingAmmo;
}

void Players::setStartingAmmo(int value)
{
    StartingAmmo = value;
}

int Players::getSleepTimeOut() const
{
    return SleepTimeOut;
}

void Players::setSleepTimeOut(int value)
{
    SleepTimeOut = value;
}

int Players::getIsKing() const
{
    return isKing;
}

void Players::setIsKing(int value)
{
    isKing = value;
}

QString Players::getSurvivalTimeString() const
{
   QString minutes = QString::number(getSurvivalTimeMinutes());
   QString seconds = QString::number(getSurvivalTimeSeconds());
   if(seconds.length() == 1) seconds = "0"+ seconds;
   return minutes + ":" + seconds;
}

QString Players::getZoneTimeString() const
{
    QString minutes = QString::number(getZoneTimeMinutes());
    QString seconds = QString::number(getZoneTimeSeconds());
    if(seconds.length() == 1) seconds = "0"+ seconds;
    return minutes + ":" + seconds;
}

int Players::getTotalTagsLanded(int thisPlayer) const
{
    int totalTagsLanded = 0;
    for (int index = 1; index < 25; index++)
    {
        if (thisPlayer != index)   //so that we dont count who we hit.
        {
            totalTagsLanded += playerInfo[index].getTagsTaken(thisPlayer);
        }
    }
    return totalTagsLanded;
}

QString Players::getHitsAndTags(int thisPlayer, int otherPlayer) const
{
    QString hitsAndTags = "-";
    int tagsLanded = 0;
    int tagsTaken  = 0;

    tagsLanded  = playerInfo[otherPlayer].getTagsTaken(thisPlayer);
    tagsTaken   = playerInfo[thisPlayer].getTagsTaken(otherPlayer);

    if(thisPlayer != otherPlayer) hitsAndTags = QString::number(0-tagsTaken) + " / " + QString::number(tagsLanded);

    return hitsAndTags;
}

int Players::getRankingInGame() const
{
    return rankingInGame;
}

void Players::setRankingInGame(int value)
{
    rankingInGame = value;
}

int Players::getGameScore() const
{
    return gameScore;
}

void Players::setGameScore(int value)
{
    gameScore = value;
}

int Players::getPlayerNumberInThisGame() const
{
    return playerNumberInThisGame;
}

void Players::setPlayerNumberInThisGame(int value)
{
    playerNumberInThisGame = value;
}

int Players::getTotalTagsTaken() const
{
    return totalTagsTaken;
}

void Players::setTotalTagsTaken(int value)
{
    totalTagsTaken = value;
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Players::streamToFile
/// \param out
///
void Players::streamToFile(QTextStream &out)
{
    out << "------------------------" << endl;
    for (int index = 0; index < 25; index++)
    {
        out << "PlayerID:" << index << endl;
        out << "Handicap: "     << playerInfo[index].Handicap << endl;
        out << "HealthTags: "   << playerInfo[index].HealthTags << endl;
        out << "MedicMode: "    << playerInfo[index].MedicMode << endl;
        out << "MegaTags: "     << playerInfo[index].MegaTags << endl;
        out << "PlayerName: "   << playerInfo[index].PlayerName << endl;
        out << "Reloads: "      << playerInfo[index].Reloads << endl;
        out << "Reloads2: "     << playerInfo[index].Reloads2 << endl;
        out << "ShieldTime: "   << playerInfo[index].ShieldTime << endl;
        out << "SlowTags: "     << playerInfo[index].SlowTags << endl;
        out << "TeamTags: "     << playerInfo[index].TeamTags << endl;
        out << "Flags1:"        << playerInfo[index].PackedFlags1 << endl;
        out << "Flags2:"        << playerInfo[index].PackedFlags2 << endl;
        out << "Flags3:"        << playerInfo[index].PackedFlags3 << endl;
        out << "StartingAmmo: " << playerInfo[index].StartingAmmo << endl;
        out << "SleepTimeOut"   << playerInfo[index].SleepTimeOut << endl;
        out << "------------------------" << endl;
    }
    out << "END_OF_PLAYER_SETTINGS" << endl;
	qDebug() << "Players::StreamToFile has left the building" << endl << endl;
}

void Players::streamFromFile(QTextStream &in)
{
    QString descriptorP = "" ;
    int playerID = 0;

    do
    {
            descriptorP = in.readLine();
            if      (descriptorP.contains("PlayerID:") )        playerID                          = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Handicap:") )        playerInfo[playerID].Handicap     = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("HealthTags:") )      playerInfo[playerID].HealthTags   = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("MedicMode:") )       playerInfo[playerID].MedicMode    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("MegaTags:") )        playerInfo[playerID].MegaTags     = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("PlayerName:") )      playerInfo[playerID].PlayerName   = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) ));
            else if (descriptorP.contains("Reloads:") )         playerInfo[playerID].Reloads      = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Reloads2:") )        playerInfo[playerID].Reloads2     = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("ShieldTime:") )      playerInfo[playerID].ShieldTime   = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("SlowTags:") )        playerInfo[playerID].SlowTags     = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("TeamTags:") )        playerInfo[playerID].TeamTags     = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Flags1:") )          playerInfo[playerID].PackedFlags1 = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Flags2:") )          playerInfo[playerID].PackedFlags2 = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Flags3:") )          playerInfo[playerID].PackedFlags3 = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("StartingAmmo:") )    playerInfo[playerID].StartingAmmo = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("SleepTimeOut:") )    playerInfo[playerID].SleepTimeOut = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
    }   while (descriptorP != "END_OF_PLAYER_SETTINGS");

//    for (int index=0; index< 25; index++)
//    {
//        qDebug() << "PlayerID:"     << index;
//        qDebug() << "Handicap:"     << playerInfo[index].Handicap;
//        qDebug() << "HealthTags:"   << playerInfo[index].HealthTags;
//        qDebug() << "MedicMode:"    << playerInfo[index].MedicMode;
//        qDebug() << "MegaTags:"     << playerInfo[index].MegaTags;
//        qDebug() << "PlayerName:"   << playerInfo[index].PlayerName;
//        qDebug() << "Reloads:"      << playerInfo[index].Reloads;
//        qDebug() << "Reloads2:"     << playerInfo[index].Reloads2;
//        qDebug() << "ShieldTime:"   << playerInfo[index].ShieldTime;
//        qDebug() << "SlowTags:"     << playerInfo[index].SlowTags;
//        qDebug() << "TeamTags:"     << playerInfo[index].TeamTags;
//        qDebug() << "PackedFlags1:" << playerInfo[index].PackedFlags1;
//        qDebug() << "PackedFlags2:" << playerInfo[index].PackedFlags2;
//        qDebug() << "PackedFlags3:" << playerInfo[index].PackedFlags3;
//        qDebug() << "StartingAmmo"  << playerInfo[index].StartingAmmo;
//        qDebug() << "SleepTimeOut"  << playerInfo[index].SleepTimeOut   << endl;
//    }
//    qDebug() << "Players::StreamFromFile has left the building" << endl << endl;
}

void Players::copyPlayerSettings(int copyFrom, int copyTo)
{
    playerInfoTemp[copyTo].Handicap     = playerInfo[copyFrom].Handicap;
    playerInfoTemp[copyTo].HealthTags   = playerInfo[copyFrom].HealthTags;
    playerInfoTemp[copyTo].MedicMode    = playerInfo[copyFrom].MedicMode;
    playerInfoTemp[copyTo].MegaTags     = playerInfo[copyFrom].MegaTags;
    playerInfoTemp[copyTo].PlayerName   = playerInfo[copyFrom].PlayerName;
    playerInfoTemp[copyTo].Reloads      = playerInfo[copyFrom].Reloads;
    playerInfoTemp[copyTo].Reloads2     = playerInfo[copyFrom].Reloads2;
    playerInfoTemp[copyTo].ShieldTime   = playerInfo[copyFrom].ShieldTime;
    playerInfoTemp[copyTo].SlowTags     = playerInfo[copyFrom].SlowTags;
    playerInfoTemp[copyTo].PackedFlags1 = playerInfo[copyFrom].PackedFlags1;
    playerInfoTemp[copyTo].PackedFlags2 = playerInfo[copyFrom].PackedFlags2;
    playerInfoTemp[copyTo].PackedFlags3 = playerInfo[copyFrom].PackedFlags3;
    playerInfoTemp[copyTo].StartingAmmo = playerInfo[copyFrom].StartingAmmo;
    playerInfoTemp[copyTo].SleepTimeOut = playerInfo[copyFrom].SleepTimeOut;
}

void Players::moveAllPlayersFromTempToMain()
{
    for(int index = 0;index <= MAX_PLAYERS; index++)
    {
        playerInfo[index].Handicap     = playerInfoTemp[index].Handicap;
        playerInfo[index].HealthTags   = playerInfoTemp[index].HealthTags;
        playerInfo[index].MedicMode    = playerInfoTemp[index].MedicMode;
        playerInfo[index].MegaTags     = playerInfoTemp[index].MegaTags;
        playerInfo[index].PlayerName   = playerInfoTemp[index].PlayerName;
        playerInfo[index].Reloads      = playerInfoTemp[index].Reloads;
        playerInfo[index].Reloads2     = playerInfoTemp[index].Reloads2;
        playerInfo[index].ShieldTime   = playerInfoTemp[index].ShieldTime;
        playerInfo[index].SlowTags     = playerInfoTemp[index].SlowTags;
        playerInfo[index].PackedFlags1 = playerInfoTemp[index].PackedFlags1;
        playerInfo[index].PackedFlags2 = playerInfoTemp[index].PackedFlags2;
        playerInfo[index].PackedFlags3 = playerInfoTemp[index].PackedFlags3;
        playerInfo[index].StartingAmmo = playerInfoTemp[index].StartingAmmo;
        playerInfo[index].SleepTimeOut = playerInfoTemp[index].SleepTimeOut;
    }
}

int Players::getPackedFlags1() const
{
    return PackedFlags1;
}

void Players::setBitFlags1(int bitNumber, bool state)
{
    PackedFlags1 ^= (-state ^ PackedFlags1) & (1 << bitNumber);
    //qDebug() << "\tFlags1: " << "\tBinary = " << displayBinary(PackedFlags1, 8);
}

int Players::getBitFlags1(int bitNumber) const
{
    return (PackedFlags1 >> bitNumber) & 1;
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
    //qDebug() << "\tFlags2: " << "\tBinary = " << displayBinary(PackedFlags2, 8);
}

int Players::getBitFlags2(int bitNumber) const
{
    return (PackedFlags2 >> bitNumber) & 1;
}

int Players::getPackedFlags3() const
{
    return PackedFlags3;
}

void Players::setPackedFlags3(int value)
{
    PackedFlags3 = value;
}

void Players::setBitFlags3(int bitNumber, bool state)
{
    PackedFlags3 ^= (-state ^ PackedFlags3) & (1 << bitNumber);
    //qDebug() << "\tFlags3: " << "\tBinary = " << displayBinary(PackedFlags3, 8);
}

int Players::getBitFlags3(int bitNumber) const
{
    return (PackedFlags3 >> bitNumber) & 1;
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
    if (Handicap != 0)
    {
        if (value == 0) value = 1;                              //Otherwise a Zero value stays zero forever.
    }
    //TODO: Values of 1 & 2 hardly change, need to work on a logarithmic curve - maybe.
    value += value * (static_cast<float>(Handicap)/10);
    if (value > 100) value = 100;
    if (value <0)   value = 0;

    return value;
}

int Players::handicapAdjust(int value, int maxValue)
{
    if (Handicap != 0)
    {
        if (value == 0) value = 1;                              //Otherwise a Zero value stays zero forever.
    }
    //TODO: Values of 1 & 2 hardly change, need to work on a logarithmic curve - maybe.
    value += value * (static_cast<float>(Handicap)/10);
    if (value > maxValue) value = maxValue;
    if (value <0)   value = 0;
    //qDebug() << "Players::handicapAdjust(int value, int maxValue)" << value;
    return value;
}

int Players::getTagsTaken(int playerNumber) const
{
    return tagsTaken[playerNumber];
}

void Players::setTagsTaken(int playerNumber, int value)
{
    tagsTaken[playerNumber] = value;
    totalTagsTaken += value;
}

int Players::getSurvivalTimeMinutes() const
{
    return survivalTimeMinutes;
}

void Players::setSurvivalTimeMinutes(int value)
{
    survivalTimeMinutes = value;
}

int Players::getSurvivalTimeSeconds() const
{
    return survivalTimeSeconds;
}

void Players::setSurvivalTimeSeconds(int value)
{
    survivalTimeSeconds = value;
}

int Players::getZoneTimeMinutes() const
{
    return zoneTimeMinutes;
}

void Players::setZoneTimeMinutes(int value)
{
    zoneTimeMinutes = value;
}

int Players::getZoneTimeSeconds() const
{
    return zoneTimeSeconds;
}

void Players::setZoneTimeSeconds(int value)
{
    zoneTimeSeconds = value;
}

int Players::copyTo() const
{
    return reportFlags;
}

void Players::setReportFlags(int value)
{
    reportFlags = value;
}
