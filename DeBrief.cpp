#include "DeBrief.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "Defines.h"

DeBrief::DeBrief(QObject *parent) : QObject(parent)
{

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
