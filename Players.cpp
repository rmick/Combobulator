#include "Players.h"
#include <QDebug>

Players playerInfo[25];

Players::Players()
{
    for (int x= 0; x<25; x++)
    {
        Handicap        = 0;
        PlayerName      = "";
        Reloads         = 100;        // 100 = Unlimited
        HealthTags      = 99;
        ShieldTime      = 60;
        MegaTags        = 15;
        SlowTags        = false;
        TeamTags        = false;
        MedicMode       = false;
        PackedFlags1    = 20;
        PackedFlags2    = 1;
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

void Players::setReloads(int value)
{
    Reloads = value;
}

QString Players::getReloadsTx() const
{
    QString _reloads = QString::number(Reloads, 10).toUpper();
    if (Reloads == 100) _reloads = "FF";
    if (_reloads.length() == 1) _reloads.prepend('0');
    return _reloads;
}

int Players::getHealthTags() const
{
    return HealthTags;
}

void Players::setHealthTags(int value)
{
    HealthTags = value;
}

QString Players::getHealthTagsTx() const
{
    QString _healthTags = QString::number(HealthTags, 10).toUpper();
    if (_healthTags.length() == 1) _healthTags.prepend('0');
    return _healthTags;
}

int Players::getShieldTime() const
{
    return ShieldTime;
}

void Players::setShieldTime(int value)
{
    ShieldTime = value;
}

QString Players::getShieldTimeTx() const
{
    QString _shieldTime = QString::number(ShieldTime, 10).toUpper();
    if (_shieldTime.length() == 1) _shieldTime.prepend('0');
    return _shieldTime;
}

int Players::getMegaTags() const
{
    return MegaTags;
}

void Players::setMegaTags(int value)
{
    MegaTags = value;
}

QString Players::getMegaTagsTx() const
{
    QString _megatags = QString::number(MegaTags, 10).toUpper();
    if (MegaTags == 100) _megatags = "FF";
    if (_megatags.length() == 1) _megatags.prepend('0');
    return _megatags;
}

bool Players::getSlowTags() const
{
    return SlowTags;
}

void Players::setSlowTags(bool value)
{
    SlowTags = value;
}

bool Players::getTeamTags() const
{
    return TeamTags;
}

void Players::setTeamTags(bool value)
{
    TeamTags = value;
}

bool Players::getMedicMode() const
{
    return MedicMode;
}

void Players::setMedicMode(bool value)
{
    MedicMode = value;
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
    QString descriptorP;
    int playerID;

    do
    {
            descriptorP = in.readLine();
            //qDebug() << descriptorP;
            if      (descriptorP.contains("PlayerID:") )        playerID                         = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("Handicap:") )        playerInfo[playerID].Handicap    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("HealthTags:") )      playerInfo[playerID].HealthTags  = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("MegaTags:") )        playerInfo[playerID].MegaTags    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("ShieldTime:") )      playerInfo[playerID].ShieldTime  = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("MedicMode:") )       playerInfo[playerID].MedicMode   = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("SlowTags:") )        playerInfo[playerID].SlowTags    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("TeamTags:") )        playerInfo[playerID].TeamTags    = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
            else if (descriptorP.contains("PlayerName:") )      playerInfo[playerID].PlayerName  = descriptorP.right((descriptorP.length() - (descriptorP.indexOf(":")+1) )).toInt();
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

void Players::setPackedFlag1_NeutraliseWhenTagged(bool state)
{
    state = false;  //TODO: add some real code here
}

void Players::setPackedFlag1_LimitedReloads(bool state)
{
    //qDebug() << "setPackedFlag1_LimitedReloads()" << state << QString::number(PackedFlags1, 2);

//    Setting the nth bit to either 1 or 0 can be achieved with the following:
//    number ^= (-x ^ number) & (1 << n);
//    Bit n will be set if x is 1, and cleared if x is 0.

//    PackedFlags1 ^= (-x ^ number) & (1 << n);

    if (state == true) PackedFlags1 = PackedFlags1 |= 1 << 6;
    else               PackedFlags1 = PackedFlags1 &= ~(1 << 6);
    //qDebug() << QString::number(PackedFlags1, 2);
}

void Players::setPackedFlags1(int value)
{
    PackedFlags1 = value;
}

QString Players::getPackedFlags1Tx() const
{
    //qDebug() << "Players::getPackedFlags1Tx() " + QString::number(PackedFlags1, 2);
    QString _packedFlags1 = QString::number(PackedFlags1, 10).toUpper();
    if (_packedFlags1.length() == 1) _packedFlags1.prepend('0');
    return _packedFlags1;


//    var flags1 = (byte) ((gameDefinition.ExtendedTagging ? 1 : 0) << 7 |
//                                     (gameDefinition.LimitedReloads ? 1 : 0) << 6 |
//                                     (gameDefinition.LimitedMega ? 1 : 0) << 5 |
//                                     (gameDefinition.TeamTags ? 1 : 0) << 4 |
//                                     (gameDefinition.MedicMode ? 1 : 0) << 3 |
//                                     (gameDefinition.SlowTags ? 1 : 0) << 2 |
//                                     (gameDefinition.GameTypeInfo.HuntThePrey ? 1 : 0) << 1 |
//                                     (gameDefinition.GameTypeInfo.ReverseHuntDirection ? 1 : 0) << 0);
}

int Players::getPackedFlags2() const
{
    //qDebug() << "Players::getPackedFlags2() " + QString::number(PackedFlags2, 2);
    return PackedFlags2;

//    var flags2 = (byte) ((gameDefinition.GameTypeInfo.Zones ? 1 : 0) << 7 |
//                         (gameDefinition.GameTypeInfo.TeamZones ? 1 : 0) << 6 |
//                         (gameDefinition.GameTypeInfo.NeutralizePlayersTaggedInZone ? 1 : 0) << 5 |
//                         (gameDefinition.GameTypeInfo.ZonesRevivePlayers ? 1 : 0) << 4 |
//                         (gameDefinition.GameTypeInfo.HospitalZones ? 1 : 0) << 3 |
//                         (gameDefinition.GameTypeInfo.ZonesTagPlayers ? 1 : 0) << 2 |
//                         (gameDefinition.TeamCount & 0x03));
}

void Players::setPackedFlags2(int value)
{
    PackedFlags2 = value;
}

QString Players::getPackedFlags2Tx() const
{
    //qDebug() << "Players::getPackedFlags2Tx() " + QString::number(PackedFlags2, 2);
    QString _packedFlags2 = QString::number(PackedFlags2, 10).toUpper();
    if (_packedFlags2.length() == 1) _packedFlags2.prepend('0');
    return _packedFlags2;
}

int Players::getTaggerID() const
{
    return TaggerID;
}

void Players::setTaggerID(int value)
{
    TaggerID = value;
}

