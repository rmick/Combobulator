#include "Game.h"
#include <QDebug>
#include <QString>
#include "Defines.h"
#include "Players.h"

Game    gameInfo;

Game	gameInfoTemp;

Game::Game()
{
    qDebug() << "Game::Game() - Constructing.......";

    GameType        = DEFAULT_GAME_TYPE;
    GameID          = DEFAULT_GAME_ID;
    GameLength      = DEFAULT_GAME_LENGTH;
    NumberOfPlayers = 24;
    isReSpawnGame   = false;
    isLTARGame      = false;
    GameName        ="LTTO";
    NameChar1       = 67;  // C
    NameChar2       = 85;  // U
    NameChar3       = 83;  // S
    NameChar4       = 84;  // T

    pointsPerTagLanded          = POINTS_PER_TAG_LANDED;
    pointsPerTagLandedNegative  = POINTS_PER_TAG_LANDED_OWN_TEAM;
    pointsPerTagTaken           = POINTS_PER_HITS_TAKEN;
    pointsPerSurvivalMinute     = POINTS_PER_SURVIVAL_MINUTE;
    pointsPerZoneMinute         = POINTS_PER_ZONE_MINUTE;
    pointsPerKingHit            = POINTS_PER_KING_HIT;
    pointsPerKingHitNegative    = POINTS_PER_OWN_KING_HIT;

    PlayerToReHost = 0;
    isSpiesTeamTagActive = false;

    team1rank = 0;
    team2rank = 0;
    team3rank = 0;

#ifdef  QT_DEBUG
	CountDownTime   = DEBUG_COUNTDOWN_TIME;
#else
	CountDownTime   = DEFAULT_COUNTDOWN_TIME;
#endif

    isThisPlayerInTheGame[0] = true;        //this is required so that Announce continues after all Taggers are hosted.
	for (int index= 1; index <= MAX_PLAYERS; index++)
    {
		isThisPlayerInTheGame[index] = false;
    }
    for (int index = 1; index < 3; index++)
    {
        PlayersInTeamByte[index] = 0;
    }
}

int Game::getGameType() const
{
    return GameType;
}

void Game::setGameType(int value)
{
    GameType = value;

    for(int index = 0; index < 25; index++)
    {
        // Reset Game Specific Flags and buttons.
        playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG, false);
        playerInfo[index].setBitFlags1(HUNT_THE_PREY_FLAG, false);
        playerInfo[index].setBitFlags1(REVERSE_HUNT_DIR_FLAG, false);
        playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, false);
        playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG, false);
        playerInfo[index].setBitFlags2(ZONES_TO_TEAMS_FLAG, false);
        playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG, false);
        playerInfo[index].setBitFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG, false);
        playerInfo[index].setBitFlags2(HOSTILE_ZONES_FLAG, false);

        // Set Game Specific Flags and Number of Teams Flags
        switch(value)
        {
        case Ltag0:
            setNumberOfTeams(0);
            break;
        case Ltag2:
            setNumberOfTeams(2);
            break;
        case Ltag3:
            setNumberOfTeams(3);
            break;

        case HideSeek2:
            playerInfo[index].setBitFlags1(HUNT_THE_PREY_FLAG, true);
            setNumberOfTeams(2);
            break;
        case HideSeek3:
            playerInfo[index].setBitFlags1(HUNT_THE_PREY_FLAG, true);
            setNumberOfTeams(3);
            break;

        case Kings2:
            setNumberOfTeams(2);
            break;
        case Kings3:
            setNumberOfTeams(3);
            break;

        case OwnZone0:
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG, true);
            playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, true);
            setNumberOfTeams(0);
            break;
        case OwnZone2:
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG, true);
            playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, true);
            setNumberOfTeams(2);
            break;
        case OwnZone3:
            playerInfo[index].setBitFlags2(CONTESTED_ZONES_FLAG, true);
            setNumberOfTeams(3);
            break;

        case Custom:
            // Nothing to do here.
            break;

        case LtarGame:
            setIsLTARGame(true);
        }
    }
}

int Game::getGameID() const
{
    return GameID;
}

void Game::setGameID(int value)
{
    GameID = value;
}

int Game::getGameLength() const
{
    return GameLength;
}

void Game::setGameLength(int value)
{
    GameLength = value;
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

    //now set the Flags2 Team bits
    bool msb, lsb;
    switch (value)
    {
        case 0:
            msb = 0;
            lsb = 1;
            break;
        case 1:
            msb = 0;
            lsb = 1;
            break;
        case 2:
            msb = 1;
            lsb = 0;
            break;
        case 3:
            msb = 1;
            lsb = 1;
            break;
        default:
            msb = 0;
            lsb = 0;
    }
    for(int x = 0; x < 25; x++)
    {
        playerInfo[x].setBitFlags2(TEAM_COUNT_MSB, msb);
        playerInfo[x].setBitFlags2(TEAM_COUNT_LSB, lsb);
    }
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
	qInfo() << "Game::setGameName() -" << value;
}

bool Game::getIsThisPlayerInTheGame(int index) const
{
    return isThisPlayerInTheGame[index];
}

void Game::setIsThisPlayerInTheGame(int index, int value)
{
    isThisPlayerInTheGame[index] = value;
}

int Game::getPlayersInTeam(int TeamNumber) const
{
    //TODO: This is BROKEN. Please fix it!
    if (gameInfo.getIsLTARGame())
    {
        qDebug() << "Game::getPlayersInTeam() - LTAR Mode - PlayersInTeamByte =" << PlayersInTeamByte[TeamNumber];
        //return PlayersInTeamByte[TeamNumber];
        return 255;
    }
    int numberOfPlayersInThisTeam = 0;
    int startingPlayer = 1 + (8*(TeamNumber-1));
    int endingPlayer = 1+(8*TeamNumber);
    for (int index = startingPlayer; index < endingPlayer; index++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(index) == true) numberOfPlayersInThisTeam++;
    }
    //qDebug() << "Game::getPlayersInTeam() - Number of players =" << numberOfPlayersInThisTeam << "in Team" << TeamNumber;

    //return numberOfPlayersInThisTeam;
    return 8;
}

void Game::setPlayersInTeamByte(int TeamNumber, int PlayerNumber, bool state)
{
    //This is wrong. Just needs to be a sum of the number of players in the team.
    //Except maybe in an LTar
    PlayersInTeamByte[TeamNumber] ^= (-state ^ PlayersInTeamByte[TeamNumber]) & (1 << (PlayerNumber-1));
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
    out << "CountDownTime:"    << CountDownTime    << endl;
    out << "SpyTeamTagsActive:"<< isSpiesTeamTagActive << endl;
    out << "LTARmode:"         << isLTARGame        << endl;
    out << "ReSpawnEnabled:"   << isReSpawnGame     << endl;
	out << "CumulativeScores:"  << cumulativeScoreMode << endl;
    out << "PointsPerTagLanded:"         << pointsPerTagLanded           << endl;
    out << "PointsPerTagLandedNegative:" << pointsPerTagLandedNegative   << endl;
    out << "PointsPerTagTaken:"          << pointsPerTagTaken            << endl;
    out << "PointsPerSurvivalMinute:"    << pointsPerSurvivalMinute      << endl;
    out << "PointsPerZoneMinute:"        <<   pointsPerZoneMinute        << endl;
    out << "PointsPerKingHit:"           <<   pointsPerKingHit           << endl;
    out << "PointsPermingHitNegative:"   <<   pointsPerKingHitNegative   << endl;
    for (int x=0; x< 25; x++)
    {
        if      (x < 10) out << " Player0" << x << ":" << isThisPlayerInTheGame[x] << endl;
        else             out << " Player"  << x << ":" << isThisPlayerInTheGame[x] << endl;;
    }

    out << "END_OF_GAME_SETTINGS" << endl;

//	qDebug() << "GameID:"           << GameID;
//	qDebug() << "GameLength:"       << GameLength;
//	qDebug() << "GameName:"         << GameName;
//	qDebug() << "GameType:"         << GameType;
//	qDebug() << "NumberOfPlayers:"  << NumberOfPlayers;
//	qDebug() << "NumberOfTeams:"    << NumberOfTeams;
//	qDebug() << "NumberOfSpies:"    << NumberOfSpies;
//	qDebug() << "CountDownTime"     << CountDownTime;
//	qDebug() << "SpyTeamTagsActive" << isSpiesTeamTagActive;
//	qDebug() << "LTARmode"          << isLTARGame;
//	qDebug() << "ReSpawnEnabled"    << isReSpawnGame;


	qDebug() << "Game::StreamToFile has left the building";
}


void Game::streamFromFile(QTextStream &in)
{
    QString descriptorG;

    do
    {
            descriptorG = in.readLine();
            //qDebug() << descriptorG << descriptorG.indexOf(":");
            if      (descriptorG.contains("GameID:") )              GameID                  = extractInteger(descriptorG);
            else if (descriptorG.contains("GameLength:") )          GameLength              = extractInteger(descriptorG);
            else if (descriptorG.contains("GameType:") )            GameType                = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfPlayers:") )     NumberOfPlayers         = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfTeams:") )       NumberOfTeams           = extractInteger(descriptorG);
            else if (descriptorG.contains("NumberOfSpies:") )       NumberOfSpies           = extractInteger(descriptorG);
            else if (descriptorG.contains("GameName:") )            GameName                = descriptorG;
            else if (descriptorG.contains(" Player") )              isThisPlayerInTheGame[descriptorG.mid(7,2).toInt()] = descriptorG.right(1).toInt();
            else if (descriptorG.contains("CountDownTime:") )       CountDownTime           = extractInteger(descriptorG);
            else if (descriptorG.contains("SpyTeamTagsActive:") )   isSpiesTeamTagActive    = extractInteger(descriptorG);
            else if (descriptorG.contains("LTARmode:") )            isLTARGame              = extractInteger(descriptorG);
			else if (descriptorG.contains("ReSpawnEnabled:") )      isReSpawnGame           = extractInteger(descriptorG);
			else if (descriptorG.contains("CumulativeScores:") )	cumulativeScoreMode		= extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPerTagLanded:") )          pointsPerTagLanded          = extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPerTagLandedNegative:") )  pointsPerTagLandedNegative  = extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPerTagTaken:") )           pointsPerTagTaken           = extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPerSurvivalMinute:") )     pointsPerSurvivalMinute     = extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPerZoneMinute:") )         pointsPerZoneMinute         = extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPerKingHit:") )            pointsPerKingHit            = extractInteger(descriptorG);
            else if (descriptorG.contains("PointsPermingHitNegative:") )    pointsPerKingHitNegative    = extractInteger(descriptorG);
    }   while (descriptorG != "END_OF_GAME_SETTINGS");

    setNumberOfTeams(NumberOfTeams);    //This is required to update the Flags2 bits.

//	qDebug() << "GameID:"           << GameID;
//	qDebug() << "GameLength:"       << GameLength;
//	qDebug() << "GameName:"         << GameName;
//	qDebug() << "GameType:"         << GameType;
//	qDebug() << "NumberOfPlayers:"  << NumberOfPlayers;
//	qDebug() << "NumberOfTeams:"    << NumberOfTeams;
//	qDebug() << "NumberOfSpies:"    << NumberOfSpies;
//	qDebug() << "CountDownTime"     << CountDownTime;
//	qDebug() << "SpyTeamTagsActive" << isSpiesTeamTagActive;
//	qDebug() << "LTARmode"          << isLTARGame;
//	qDebug() << "ReSpawnEnabled"    << isReSpawnGame;
//	qDebug() << "PlayersInGame;";
//	for (int x=0; x< 25; x++)
//	{
//		qDebug() << "  Player"  << x << ":" << isThisPlayerInTheGame[x];
//	}

//	qDebug() << "Game::StreamFromFile has left the building" << endl;
}

int Game::getCountDownTime() const
{
    return CountDownTime;
}

void Game::setCountDownTime(int value)
{
    CountDownTime = value;
}

char Game::getNameChar1() const
{
    return NameChar1;
}

void Game::setNameChar1(char value)
{
    NameChar1 = value;
}

char Game::getNameChar2() const
{
    return NameChar2;
}

void Game::setNameChar2(char value)
{
    NameChar2 = value;
}

char Game::getNameChar3() const
{
    return NameChar3;
}

void Game::setNameChar3(char value)
{
    NameChar3 = value;
}

char Game::getNameChar4() const
{
    return NameChar4;
}

void Game::setNameChar4(char value)
{
    NameChar4 = value;
}

bool Game::getIsSpiesTeamTagActive() const
{
    return isSpiesTeamTagActive;
}

void Game::setIsSpiesTeamTagActive(bool value)
{
    isSpiesTeamTagActive = value;
}

int Game::getTotalNumberOfPlayersInGame() const
{
    int count = 0;
    for (int x = 1; x<25;x++)
    {
        if(getIsThisPlayerInTheGame(x)) count++;
    }
    return count;
}

int Game::getPlayerToReHost() const
{
    return PlayerToReHost;
}

void Game::setPlayerToReHost(int value)
{
    PlayerToReHost = value;
}

bool Game::getIsLTARGame() const
{
    return isLTARGame;
}

void Game::setIsLTARGame(bool value)
{
    isLTARGame = value;
}

bool Game::getIsReSpawnGame() const
{
    return isReSpawnGame;
}

void Game::setIsReSpawnGame(bool value)
{
    isReSpawnGame = value;
}

int Game::getNumberOfPlayersInGame()
{
    numberOfPlayersInGame = 0;
    for (int index = 1; index < 25; index++)
    {
        if(isThisPlayerInTheGame[index] == true) numberOfPlayersInGame++;
    }
    return numberOfPlayersInGame;
}

int Game::getPointsPerTagLanded() const
{
    return pointsPerTagLanded;
}

void Game::setPointsPerTagLanded(int value)
{
    pointsPerTagLanded = value;
}

int Game::getPointsPerTagLandedNegative() const
{
    return pointsPerTagLandedNegative;
}

void Game::setPointsPerTagLandedNegative(int value)
{
    pointsPerTagLandedNegative = value;
}

int Game::getPointsPerTagTaken() const
{
    return pointsPerTagTaken;
}

void Game::setPointsPerTagTaken(int value)
{
    pointsPerTagTaken = value;
}

int Game::getPointsPerKingHit() const
{
    return pointsPerKingHit;
}

void Game::setPointsPerKingHit(int value)
{
    pointsPerKingHit = value;
}

int Game::getPointsPerKingHitNegative() const
{
    return pointsPerKingHitNegative;
}

void Game::setPointsPerKingHitNegative(int value)
{
    pointsPerKingHitNegative = value;
}

int Game::getPointsPerSurvivalMinute() const
{
    return pointsPerSurvivalMinute;
}

void Game::setPointsPerSurvivalMinute(int value)
{
    pointsPerSurvivalMinute = value;
}

int Game::getPointsPerZoneMinute() const
{
    return pointsPerZoneMinute;
}

void Game::setPointsPerZoneMinute(int value)
{
    pointsPerZoneMinute = value;
}

int Game::getTeam1rank() const
{
    return team1rank;
}

void Game::setTeam1rank(int value)
{
    team1rank = value;
}

int Game::getTeam2rank() const
{
    return team2rank;
}

void Game::setTeam2rank(int value)
{
    team2rank = value;
}

int Game::getTeam3rank() const
{
    return team3rank;
}

void Game::setTeam3rank(int value)
{
    team3rank = value;
}

bool Game::getIsIndoorViewMode() const
{
    return isIndoorViewMode;
}

void Game::setIsIndoorViewMode(bool value)
{
    isIndoorViewMode = value;
}

bool Game::getCumulativeScoreMode() const
{
    return cumulativeScoreMode;
}

void Game::setCumulativeScoreMode(bool value)
{
    cumulativeScoreMode = value;
}

int Game::extractInteger(QString &dG)
{
    return dG.right((dG.length() - (dG.indexOf(":")+1) )).toInt();      //extracts all the chars to right of the ":" and convert to an Int
}
