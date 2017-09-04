#include "DeBrief.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "Defines.h"

DeBrief::DeBrief(QObject *parent) : QObject(parent)
{
    connect(&lttoComms, SIGNAL(TagSummaryReceived(int,int,int,int,int,int,int,int)), this, SLOT(ReceiveTagSummary(int,int,int,int,int,int,int,int)) );

    deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;

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

    //  Request Team1/2/3 reports
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
        deBriefPlayer = playerToInterogate - 1;       //currentPlayer is 0 based.
    }

    if(playerToInterogate > 9 && playerToInterogate < 17)
    {
        deBriefTeam = 2;
        deBriefPlayer = playerToInterogate - 9;       //currentPlayer is 0 based.
    }

    if(playerToInterogate > 16 && playerToInterogate < 25)
    {
        deBriefTeam = 3;
        deBriefPlayer = playerToInterogate - 17;       //currentPlayer is 0 based.
    }

    qDebug() << "DeBrief::RequestTagReports() -" << playerToInterogate << ":" << deBriefTeam << deBriefPlayer;

    lttoComms.sendPacket(PACKET, REQUEST_TAG_REPORT);
    lttoComms.sendPacket(DATA, gameInfo.getGameID());
    lttoComms.sendPacket(DATA, (deBriefTeam << 4) + deBriefPlayer);
    lttoComms.sendPacket(DATA, deBriefMessageType);
    lttoComms.sendPacket(CHECKSUM);

    // this should get an answer from a tagger.
}

void DeBrief::ReceiveTagSummary(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags)
{
    int team    = teamAndPlayer >> 4;
    int player  = (teamAndPlayer - team) + 1;

    playerInfo[currentPlayer].setTagsTaken          (lttoComms.ConvertBCDtoDec(tagsTaken));
    playerInfo[currentPlayer].setSurvivalTimeMinutes(lttoComms.ConvertBCDtoDec(survivedMinutes));
    playerInfo[currentPlayer].setSurvivalTimeSeconds(lttoComms.ConvertBCDtoDec(survivedSeconds));
    playerInfo[currentPlayer].setZoneTimeMinutes    (lttoComms.ConvertBCDtoDec(zoneTimeMinutes));
    playerInfo[currentPlayer].setZoneTimeSeconds    (lttoComms.ConvertBCDtoDec(zoneTimeSeconds));
    playerInfo[currentPlayer].setTagFlags           (flags);

    isTeam1TagReportDue = (flags >> 2) & 1;
    isTeam1TagReportDue = (flags >> 3) & 1;
    isTeam1TagReportDue = (flags >> 4) & 1;

    qDebug() << "DeBrief::ReceiveTagSummary() - Game" << game << "Team" << team << "Player" << player << "aka-" << currentPlayer;
    qDebug() << "Tags taken =" << playerInfo[currentPlayer].getTagsTaken();
    qDebug() << "SurvivalTime =" << playerInfo[currentPlayer].getSurvivalTimeMinutes() << ":" << playerInfo[currentPlayer].getSurvivalTimeSeconds();
    qDebug() << "ZoneTime =" << playerInfo[currentPlayer].getZoneTimeMinutes() << ":" << playerInfo[currentPlayer].getZoneTimeSeconds();
    qDebug() << "Flags" << playerInfo[currentPlayer].getTagFlags() << "Team1:" << isTeam1TagReportDue << "\tTeam2:" << isTeam2TagReportDue << "\tTeam3:" << isTeam3TagReportDue;

}

