#include "DeBrief.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "Defines.h"

DeBrief::DeBrief(QObject *parent) : QObject(parent)
{
    connect(&lttoComms, SIGNAL(TagSummaryReceived(int,int,int,int,int,int,int,int)), this, SLOT(ReceiveTagSummary(int,int,int,int,int,int,int,int)) );
    //connect(&lttoComms, SIGNAL(Team1TagReportReceived(int, int, int[])), this, SLOT(ReceiveTeam1Report(int,int,int[])));
    connect(&lttoComms, SIGNAL(Team1TagReportReceived(int,int,int,int,int,int,int,int,int,int)), this, SLOT(Team1TagReportReceived(int,int,int,int,int,int,int,int,int,int)));
    connect(&lttoComms, SIGNAL(Team2TagReportReceived(int,int,int,int,int,int,int,int,int,int)), this, SLOT(Team2TagReportReceived(int,int,int,int,int,int,int,int,int,int)));
    connect(&lttoComms, SIGNAL(Team3TagReportReceived(int,int,int,int,int,int,int,int,int,int)), this, SLOT(Team3TagReportReceived(int,int,int,int,int,int,int,int,int,int)));

    deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;
    currentPlayer = 0;
    isPlayerDeBriefed = false;
    maxTeamNum  = 0;
    deBriefTeam = 1;

    switch (gameInfo.getNumberOfTeams())
    {
    case 0:
    case 1:
        maxTeamNum = 1;
        break;
    case 2:
        maxTeamNum = 2;
        break;
    case 3:
        maxTeamNum = 3;
        break;
    }

    //pseudoCode

    //  Get currentPlayer
    //  Request Tag Report (Team, Player, Type) - 0x31
    //      Type = Tag Summary

    //      Wait for reply of Tag Summary (0x40)
    //      Decode it
    //      Check Flags for whether there is Team1/2/3 reports

    //  No need to request Team1/2/3 reports - these are sent if requested in 0x31 packet.
    //      Wait for Team Tag Reports (0x41 Team1, 0x42 Team2, 0x43 Team3)
    //      Process all the data and push to an array

    //      Repeat up to 24 times.

    //  Send Rank Report (0x32) for each of the 3 teams (or once with no teams for solo games)
    //

}



void DeBrief::RequestTagReports(int playerToInterogate)
{   
    if (lttoComms.getDontAnnounceGame()) return;

    currentPlayer = playerToInterogate;

    if(playerToInterogate < 9)
    {
        deBriefTeam = 1;
        deBriefPlayer = playerToInterogate - 1;       //deBriefPlayer is 0 based.
    }

    if(playerToInterogate > 9 && playerToInterogate < 17)
    {
        deBriefTeam = 2;
        deBriefPlayer = playerToInterogate - 9;       //deBriefPlayer is 0 based.
    }

    if(playerToInterogate > 16 && playerToInterogate < 25)
    {
        deBriefTeam = 3;
        deBriefPlayer = playerToInterogate - 17;       //deBriefPlayer is 0 based.
    }

    int teamAndPlayerByte;
    if(gameInfo.getIsLTARGame() )
    {
        teamAndPlayerByte = (deBriefTeam << 3) + deBriefPlayer;
    }
    else
    {
        teamAndPlayerByte = (deBriefTeam << 4) + deBriefPlayer;
    }
    qDebug() << "DeBrief::RequestTagReports() -" << playerToInterogate << ":" << deBriefTeam << deBriefPlayer << "MessageType" << deBriefMessageType;

    lttoComms.sendPacket(PACKET, REQUEST_TAG_REPORT);
    lttoComms.sendPacket(DATA, gameInfo.getGameID());
    lttoComms.sendPacket(DATA, teamAndPlayerByte);
    lttoComms.sendPacket(DATA, deBriefMessageType);
    lttoComms.sendPacket(CHECKSUM);
}

void DeBrief::ReceiveTagSummary(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags)
{
    qDebug() << "\n\tDeBrief::ReceiveTagSummary() from Player:" << currentPlayer;
    if(gameInfo.getGameID() != game)
    {
        qDebug() << "DeBrief::ReceiveTagSummary: GameID does not match !";
        return;
    }

    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    playerInfo[currentPlayer].setTagsTaken       (0, lttoComms.ConvertBCDtoDec(tagsTaken));  //Player 0 is global
    playerInfo[currentPlayer].setSurvivalTimeMinutes(lttoComms.ConvertBCDtoDec(survivedMinutes));
    playerInfo[currentPlayer].setSurvivalTimeSeconds(lttoComms.ConvertBCDtoDec(survivedSeconds));
    playerInfo[currentPlayer].setZoneTimeMinutes    (lttoComms.ConvertBCDtoDec(zoneTimeMinutes));
    playerInfo[currentPlayer].setZoneTimeSeconds    (lttoComms.ConvertBCDtoDec(zoneTimeSeconds));
    playerInfo[currentPlayer].setReportFlags           (flags);

    if(deBriefMessageType == REQUEST_ALL_TEAM_REPORTS_BITS)
    {
        isTeam1TagReportDue = (flags & 2);
        isTeam2TagReportDue = (flags & 4);
        isTeam3TagReportDue = (flags & 8);

        if (isTeam1TagReportDue == false && isTeam2TagReportDue == false && isTeam3TagReportDue == false)
        {
            setIsPlayerDeBriefed(true);
            deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;
            qDebug() << "---------------------" << endl;
        }
    }
    else deBriefMessageType = REQUEST_ALL_TEAM_REPORTS_BITS;

    qDebug() << "DeBrief::ReceiveTagSummary() - Game" << game  << currentPlayer;
    qDebug() << "\tTags taken =" << playerInfo[currentPlayer].getTagsTaken(0);
    qDebug() << "\tSurvivalTime =" << playerInfo[currentPlayer].getSurvivalTimeMinutes() << ":" << playerInfo[currentPlayer].getSurvivalTimeSeconds();
    qDebug() << "\tZoneTime =" << playerInfo[currentPlayer].getZoneTimeMinutes() << ":" << playerInfo[currentPlayer].getZoneTimeSeconds();
    qDebug() << "\tFlags" << playerInfo[currentPlayer].getReportFlags() << "\tTeam1:" << isTeam1TagReportDue << "\tTeam2:" << isTeam2TagReportDue << "\tTeam3:" << isTeam3TagReportDue;
}

void DeBrief::Team1TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8)
{
    if(gameInfo.getGameID() != game) return;
    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    qDebug() << "\n\tDeBrief::ReceiveTeam1Report() - " << tagsP1 << tagsP2 << tagsP3 << tagsP4 << tagsP5 << tagsP6 << tagsP7 << tagsP8;

    playerInfo[currentPlayer].setTagsTaken(1, tagsP1);
    playerInfo[currentPlayer].setTagsTaken(2, tagsP2);
    playerInfo[currentPlayer].setTagsTaken(3, tagsP3);
    playerInfo[currentPlayer].setTagsTaken(4, tagsP4);
    playerInfo[currentPlayer].setTagsTaken(5, tagsP5);
    playerInfo[currentPlayer].setTagsTaken(6, tagsP6);
    playerInfo[currentPlayer].setTagsTaken(7, tagsP7);
    playerInfo[currentPlayer].setTagsTaken(8, tagsP8);

    if (isTeam2TagReportDue == false && isTeam3TagReportDue == false)
    {
        setIsPlayerDeBriefed(true);
        deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;
        qDebug() << "---------------------" << endl;
    }
}

void DeBrief::Team2TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8)
{
    if(gameInfo.getGameID() != game) return;
    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    qDebug() << "\n\tDeBrief::ReceiveTeam2Report() - " << tagsP1 << tagsP2 << tagsP3 << tagsP4 << tagsP5 << tagsP6 << tagsP7 << tagsP8;

    playerInfo[currentPlayer].setTagsTaken(9,  tagsP1);
    playerInfo[currentPlayer].setTagsTaken(10, tagsP2);
    playerInfo[currentPlayer].setTagsTaken(11, tagsP3);
    playerInfo[currentPlayer].setTagsTaken(12, tagsP4);
    playerInfo[currentPlayer].setTagsTaken(13, tagsP5);
    playerInfo[currentPlayer].setTagsTaken(14, tagsP6);
    playerInfo[currentPlayer].setTagsTaken(15, tagsP7);
    playerInfo[currentPlayer].setTagsTaken(16, tagsP8);

    if (isTeam3TagReportDue == false)
    {
        setIsPlayerDeBriefed(true);
        deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;
        qDebug() << "---------------------" << endl;
    }
}

void DeBrief::Team3TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8)
{
    if(gameInfo.getGameID() != game) return;
    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    qDebug() << "\n\tDeBrief::ReceiveTeam3Report() - " << tagsP1 << tagsP2 << tagsP3 << tagsP4 << tagsP5 << tagsP6 << tagsP7 << tagsP8;

    playerInfo[currentPlayer].setTagsTaken(17, tagsP1);
    playerInfo[currentPlayer].setTagsTaken(18, tagsP2);
    playerInfo[currentPlayer].setTagsTaken(19, tagsP3);
    playerInfo[currentPlayer].setTagsTaken(20, tagsP4);
    playerInfo[currentPlayer].setTagsTaken(21, tagsP5);
    playerInfo[currentPlayer].setTagsTaken(22, tagsP6);
    playerInfo[currentPlayer].setTagsTaken(23, tagsP7);
    playerInfo[currentPlayer].setTagsTaken(24, tagsP8);

    //deBriefMessageType = REQUEST_TAG_REPORT;
    setIsPlayerDeBriefed(true);
    deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;
    qDebug() << "---------------------" << endl;
}

void DeBrief::sendRankReport()
{
    qDebug() << "DeBrief::sendRankReport()";
    int teamPlayerByte = 0;
    if(gameInfo.getNumberOfTeams() == 0)
    {
        teamPlayerByte = 0;
    }
    else
    {
        //set Teams/Players
    }

    lttoComms.sendPacket(PACKET, gameInfo.getGameID());
    lttoComms.sendPacket(DATA, teamPlayerByte);
    lttoComms.sendPacket(DATA, 1);
    lttoComms.sendPacket(DATA, 2);
    lttoComms.sendPacket(DATA, 3);
    lttoComms.sendPacket(DATA, 4);
    lttoComms.sendPacket(DATA, 5);
    lttoComms.sendPacket(DATA, 6);
    lttoComms.sendPacket(DATA, 7);
    lttoComms.sendPacket(DATA, 8);
    lttoComms.sendPacket(CHECKSUM);
}

bool DeBrief::decodeTeamAndPlayer(int teamAndPlayer)
{
    bool result = false;
    int team = 0;
    int player = 0;
    if (gameInfo.getIsLTARGame())
    {
        team = teamAndPlayer >> 3;
        player = (teamAndPlayer & 7) + 1;
    }
    else
    {
        team = teamAndPlayer >> 4;
        player = (teamAndPlayer & 15) + 1;
    }

    int checkPlayer;
    switch (team)
    {
    case 1:
        checkPlayer = player;
        break;
    case 2:
        checkPlayer = player+8;
        break;
    case 3:
        checkPlayer = player+16;
        break;
    }
    if (checkPlayer == currentPlayer)
    {
        result = true;
    }
    else
    {
        qDebug() << "DeBrief::ReceiveTagSummary: Player does not match !";
        result = false;
    }
    return result;
}

bool DeBrief::getIsPlayerDeBriefed() const
{
    return isPlayerDeBriefed;
}

void DeBrief::setIsPlayerDeBriefed(bool value)
{
    isPlayerDeBriefed = value;
}
