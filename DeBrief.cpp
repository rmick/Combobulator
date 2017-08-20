#include "DeBrief.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "Defines.h"

DeBrief::DeBrief(QObject *parent) : QObject(parent)
{
    //pseudoCode

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

void DeBrief::RequestTagReports()
{
    for(int playerToInterogate = 0; playerToInterogate < 24; playerToInterogate++)
    {
        int teamNum;

        switch (gameInfo.getNumberOfTeams())
        {
        case 0:
        case 1:
            teamNum = 1;
            break;
        case 2:
            teamNum = 2;
            break;
        case 3:
            teamNum = 3;
            break;
        }

        lttoComms.sendPacket(REQUEST_TAG_REPORT);
        lttoComms.sendPacket(gameInfo.getGameID());
        lttoComms.sendPacket((teamNum << 4) + playerToInterogate);
        lttoComms.sendPacket(REQUEST_TAG_SUMMARY_BIT);
        lttoComms.sendPacket(CHECKSUM);
    }
}
