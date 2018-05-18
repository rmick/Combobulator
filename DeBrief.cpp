#include "DeBrief.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "Defines.h"
#include "HostGameWindow.h"
#include <QMultiMap>



DeBrief::DeBrief(QObject *parent) : QObject(parent)
{
	lttoComms = LttoComms::getInstance();

	connect(lttoComms, SIGNAL(TagSummaryReceived(int,int,int,int,int,int,int,int)),				this, SLOT(ReceiveTagSummary(int,int,int,int,int,int,int,int)) );
	connect(lttoComms, SIGNAL(Team1TagReportReceived(int,int,int,int,int,int,int,int,int,int)), this, SLOT(Team1TagReportReceived(int,int,int,int,int,int,int,int,int,int)));
	connect(lttoComms, SIGNAL(Team2TagReportReceived(int,int,int,int,int,int,int,int,int,int)), this, SLOT(Team2TagReportReceived(int,int,int,int,int,int,int,int,int,int)));
	connect(lttoComms, SIGNAL(Team3TagReportReceived(int,int,int,int,int,int,int,int,int,int)), this, SLOT(Team3TagReportReceived(int,int,int,int,int,int,int,int,int,int)));

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
    //      Flags = 1 !!!!

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



void DeBrief::RequestTagReports()
{   
	if (lttoComms->getDontAnnounceGame())
    {
        if(timeOutCount++ == 3)
        {
			lttoComms->setDontAnnounceGame(false);
            emit SendToHGWlistWidget("Listening...." + QString::number(timeOutCount));
            qDebug() << "Listening....." << timeOutCount;
            return;
        }

    }


    //--------------------------------------
    //TODO: Deal with Spies and Kings.......
    //--------------------------------------
    //--------------------------------------
    //--------------------------------------

    int playerToDeBrief = playerInfo[currentPlayer].getPlayerNumberInThisGame();

    if(playerToDeBrief <= 8)
    {
        deBriefTeam = 1;
        deBriefPlayer = playerToDeBrief - 1;      //deBriefPlayer is 0 based.
    }

    if(playerToDeBrief >= 9 && playerToDeBrief <= 16)
    {
        deBriefTeam = 2;
        deBriefPlayer = playerToDeBrief - 9;      //deBriefPlayer is 0 based.
    }

    if(playerToDeBrief >= 17 && playerToDeBrief <= 24)
    {
        deBriefTeam = 3;
        deBriefPlayer = playerToDeBrief - 17;     //deBriefPlayer is 0 based.
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
    qDebug() << "\nDeBrief::RequestTagReports() -" << currentPlayer << playerToDeBrief << ":" << deBriefTeam << deBriefPlayer << "MessageType" << deBriefMessageType;
    emit SendToHGWlistWidget("Debriefing Player =" + QString::number(currentPlayer) + ", MessageType:" + QString::number(deBriefMessageType));

	lttoComms->sendPacket(PACKET, REQUEST_TAG_REPORT);
	lttoComms->sendPacket(DATA, gameInfo.getGameID());
	lttoComms->sendPacket(DATA, teamAndPlayerByte);
	lttoComms->sendPacket(DATA, deBriefMessageType);
	lttoComms->sendPacket(CHECKSUM);


}

void DeBrief::ReceiveTagSummary(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags)
{
    qDebug() << "\n\tDeBrief::ReceiveTagSummary() from Player:" << currentPlayer;
    emit SendToHGWlistWidget("\tReceived TagSummary from Player:" + QString::number(currentPlayer));

    if(gameInfo.getGameID() != game)
    {
        qDebug() << "DeBrief::ReceiveTagSummary: GameID does not match !";
        return;
    }

    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false)
    {
        qDebug() << "Debrief::ReceiveTagSummary - Player does not match !";
        return;
    }

	lttoComms->setDontAnnounceGame(true);

	playerInfo[currentPlayer].setTagsTaken       (0, lttoComms->ConvertBCDtoDec(tagsTaken));  //Player 0 is global
	playerInfo[currentPlayer].setSurvivalTimeMinutes(lttoComms->ConvertBCDtoDec(survivedMinutes));
	playerInfo[currentPlayer].setSurvivalTimeSeconds(lttoComms->ConvertBCDtoDec(survivedSeconds));
	playerInfo[currentPlayer].setZoneTimeMinutes    (lttoComms->ConvertBCDtoDec(zoneTimeMinutes));
	playerInfo[currentPlayer].setZoneTimeSeconds    (lttoComms->ConvertBCDtoDec(zoneTimeSeconds));
    playerInfo[currentPlayer].setReportFlags           (flags);

    //Set the flags based on what we know we should receive.
    //NB. Packets can arrive out of order (if one is corrupted), so we check to see if we have already received these packets.
    isTeam1TagReportDue = (flags & 2);
    isTeam2TagReportDue = (flags & 4);
    isTeam3TagReportDue = (flags & 8);

    //Set 'isTeam?TagReportReceived' to true for any packets NOT expected to be received.
    if(isTeam1TagReportDue == false)        isTeam1TagReportReceived = true;
    if(isTeam2TagReportDue == false)        isTeam2TagReportReceived = true;
    if(isTeam3TagReportDue == false)        isTeam3TagReportReceived = true;

    isSummaryTagReportReceived = true;

    qDebug() << "DeBrief::ReceiveTagSummary() - Game" << game  << currentPlayer;
    qDebug() << "\tTags taken =" << playerInfo[currentPlayer].getTagsTaken(0);
    qDebug() << "\tSurvivalTime =" << playerInfo[currentPlayer].getSurvivalTimeMinutes() << ":" << playerInfo[currentPlayer].getSurvivalTimeSeconds();
    qDebug() << "\tZoneTime =" << playerInfo[currentPlayer].getZoneTimeMinutes() << ":" << playerInfo[currentPlayer].getZoneTimeSeconds();
    qDebug() << "\tFlags" << playerInfo[currentPlayer].copyTo() << "\tTeam1:" << isTeam1TagReportDue << "\tTeam2:" << isTeam2TagReportDue << "\tTeam3:" << isTeam3TagReportDue;
    SendToHGWlistWidget("DeBrief::ReceiveTagSummary() - Game" +QString::number(game) + ", Player" +QString::number(currentPlayer));
    SendToHGWlistWidget("\tTags taken =" +QString::number(playerInfo[currentPlayer].getTagsTaken(0)));
}

void DeBrief::Team1TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8)
{
    if(gameInfo.getGameID() != game) return;
    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    qDebug() << "\nDeBrief::ReceiveTeam1Report() - " << tagsP1 << tagsP2 << tagsP3 << tagsP4 << tagsP5 << tagsP6 << tagsP7 << tagsP8;
    SendToHGWlistWidget("DeBrief::ReceiveTeam1Report() -" + QString::number(tagsP1) + QString::number(tagsP2) + QString::number(tagsP3) + QString::number(tagsP4) + QString::number(tagsP5) + QString::number(tagsP6) + QString::number(tagsP7) + QString::number(tagsP8) );

	lttoComms->setDontAnnounceGame(true);

    playerInfo[currentPlayer].setTagsTaken(1, tagsP1);
    playerInfo[currentPlayer].setTagsTaken(2, tagsP2);
    playerInfo[currentPlayer].setTagsTaken(3, tagsP3);
    playerInfo[currentPlayer].setTagsTaken(4, tagsP4);
    playerInfo[currentPlayer].setTagsTaken(5, tagsP5);
    playerInfo[currentPlayer].setTagsTaken(6, tagsP6);
    playerInfo[currentPlayer].setTagsTaken(7, tagsP7);
    playerInfo[currentPlayer].setTagsTaken(8, tagsP8);

    isTeam1TagReportReceived = true;

//    if (isTeam2TagReportDue == false && isTeam3TagReportDue == false)
//    {
//        setIsPlayerDeBriefed(true);
//        deBriefMessageType = REQUEST_TAG_SUMMARY_BIT;
//        qDebug() << "---------------------" << endl;
//    }

    //deBriefMessageType = REQUEST_TEAM2_REPORT_BIT;
}

void DeBrief::Team2TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8)
{
    if(gameInfo.getGameID() != game) return;
    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    qDebug() << "\nDeBrief::ReceiveTeam2Report() - " << tagsP1 << tagsP2 << tagsP3 << tagsP4 << tagsP5 << tagsP6 << tagsP7 << tagsP8;
    SendToHGWlistWidget("DeBrief::ReceiveTeam2Report() -" + QString::number(tagsP1) + QString::number(tagsP2) + QString::number(tagsP3) + QString::number(tagsP4) + QString::number(tagsP5) + QString::number(tagsP6) + QString::number(tagsP7) + QString::number(tagsP8) );

	lttoComms->setDontAnnounceGame(true);

    playerInfo[currentPlayer].setTagsTaken(9,  tagsP1);
    playerInfo[currentPlayer].setTagsTaken(10, tagsP2);
    playerInfo[currentPlayer].setTagsTaken(11, tagsP3);
    playerInfo[currentPlayer].setTagsTaken(12, tagsP4);
    playerInfo[currentPlayer].setTagsTaken(13, tagsP5);
    playerInfo[currentPlayer].setTagsTaken(14, tagsP6);
    playerInfo[currentPlayer].setTagsTaken(15, tagsP7);
    playerInfo[currentPlayer].setTagsTaken(16, tagsP8);

    isTeam2TagReportReceived = true;
}

void DeBrief::Team3TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8)
{
    if(gameInfo.getGameID() != game) return;
    // Check if TeamAndPlayer matches currentPlayer and bail if not.
    if (decodeTeamAndPlayer(teamAndPlayer) == false) return;

    qDebug() << "\nDeBrief::ReceiveTeam3Report() - " << tagsP1 << tagsP2 << tagsP3 << tagsP4 << tagsP5 << tagsP6 << tagsP7 << tagsP8;
    SendToHGWlistWidget("DeBrief::ReceiveTeam3Report() -" + QString::number(tagsP1) + QString::number(tagsP2) + QString::number(tagsP3) + QString::number(tagsP4) + QString::number(tagsP5) + QString::number(tagsP6) + QString::number(tagsP7) + QString::number(tagsP8) );

	lttoComms->setDontAnnounceGame(true);

    playerInfo[currentPlayer].setTagsTaken(17, tagsP1);
    playerInfo[currentPlayer].setTagsTaken(18, tagsP2);
    playerInfo[currentPlayer].setTagsTaken(19, tagsP3);
    playerInfo[currentPlayer].setTagsTaken(20, tagsP4);
    playerInfo[currentPlayer].setTagsTaken(21, tagsP5);
    playerInfo[currentPlayer].setTagsTaken(22, tagsP6);
    playerInfo[currentPlayer].setTagsTaken(23, tagsP7);
    playerInfo[currentPlayer].setTagsTaken(24, tagsP8);

    isTeam3TagReportReceived = true;
}

void DeBrief::sendRankReport()
{
    //calculateRankings();

    qDebug() << "DeBrief::sendRankReport()";
    int teamPlayerByte  = 0;
    int loopCount       = 1;

    qDebug() << "\nDeBrief::sendRankReport()  - Team 1 Rank =" << gameInfo.getTeam1rank();
    qDebug() <<   "DeBrief::sendRankReport()  - Team 2 Rank =" << gameInfo.getTeam2rank();
    qDebug() <<   "DeBrief::sendRankReport()  - Team 3 Rank =" << gameInfo.getTeam3rank();

    //There are 3 rankReport messages, for players 1-8, 9-1, 17-24
    for (int index = 1; index <4; index++)
    {
         qDebug() << "DeBrief::sendRankReport() - Loop" << loopCount << "\tTeam:" << index;

         //Set the TeamPlayerByte
         teamPlayerByte = index << 4;

         //Add TeamRanks for non solo games only.
         if(gameInfo.getNumberOfTeams() != 0)
         {
             if         (index == 1)
             {
                 teamPlayerByte += gameInfo.getTeam1rank();
             }
             else if    (index == 2)
             {
                 teamPlayerByte += gameInfo.getTeam2rank();
             }
             else if    (index == 3)
             {
                 teamPlayerByte += gameInfo.getTeam3rank();
             }
         }
         qDebug() << teamPlayerByte;

//         //Send the message
		 lttoComms->sendPacket(PACKET, SEND_RANK_REPORT);
		 lttoComms->sendPacket(DATA, gameInfo.getGameID());
		 lttoComms->sendPacket(DATA, teamPlayerByte);
		 lttoComms->sendPacket(DATA, playerInfo[1 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[2 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[3 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[4 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[5 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[6 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[7 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(DATA, playerInfo[8 + ((loopCount-1)*8)].getRankingInGame());
		 lttoComms->sendPacket(CHECKSUM);
		 lttoComms->nonBlockingDelay(100);

        //reset index to 1 and exit if done 3x5 times
        if (index == 3 && loopCount < 5)
        {
            loopCount++;
            index = 1;
        }
    }
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

    int checkPlayer = 0;
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

void DeBrief::calculateScores()
{
    for (int index = 1; index <= MAX_PLAYERS; index++)
    {
        int score = 0;
        score += playerInfo[index].getTotalTagsLanded(index)            * gameInfo.getPointsPerTagLanded();
        score += playerInfo[index].getTotalTagsTaken()                  * gameInfo.getPointsPerTagTaken();
        score += playerInfo[index].getZoneTimeMinutes()                 * gameInfo.getPointsPerZoneMinute();
        score += playerInfo[index].getZoneTimeSeconds()                 *(gameInfo.getPointsPerZoneMinute() / 60.0);
        score += playerInfo[index].getSurvivalTimeMinutes()             * gameInfo.getPointsPerSurvivalMinute();
        score += playerInfo[index].getSurvivalTimeSeconds()             *(gameInfo.getPointsPerSurvivalMinute() / 60.0);
         //TODO: playerInfo[index].setKingsTagsLanded()
        //       playerInfo[index].setOwnKingTagsLanded()
        //       playerInfo[index].setOwnTeamTagsLanded()
        //       do not exist yet.
//        score += playerInfo[index].getKingTagsLanded()                 * gameInfo.getPointsPerKingHit();
//        score += playerInfo[index].getOwnKingTagsLanded()              * gameInfo.getPointsPerZoneMinute();
//        score += playerInfo[index].getOwnTeamTagsLanded()              * gameInfo.getPointsPerTagLandedNegative();

        playerInfo[index].setGameScore(score);
    }
}

void DeBrief::calculateRankings()
{
    int counter = MAX_PLAYERS;
    int team1rankTotals = 0;
    int team2rankTotals = 0;
    int team3rankTotals = 0;

    //Create a QMultimap with scores as key, and playerNumber as value.
    QMultiMap<int, int> scoresTable;
    for (int index = 1; index <= MAX_PLAYERS; index++)
    {
        scoresTable.insert(playerInfo[index].getGameScore(), index);
    }

    //extract the ranking order and save to playerInfo.
    QMapIterator<int, int> scoresIterator(scoresTable);
    while (scoresIterator.hasNext())
    {
        scoresIterator.next();
        playerInfo[scoresIterator.value()].setRankingInGame(counter);
        counter--;
    }

    //Adjust ranks to 1224 system
    int currentRankedPlayer = 0;

    for (int rankIndex = MAX_PLAYERS; rankIndex >= 1; rankIndex--)
    {
        //find the player whose rank == rankIndex
        for (int thisPlayer = 1; thisPlayer <= MAX_PLAYERS; thisPlayer++)
        {
            if(playerInfo[thisPlayer].getRankingInGame() == rankIndex)
            {
                currentRankedPlayer = thisPlayer;
            }
        }
        //get their score
        int scoreToCheck = playerInfo[currentRankedPlayer].getGameScore();

        //does anyone else have the same score? if so match the rank
        for (int playerToCompare = 1; playerToCompare <= MAX_PLAYERS; playerToCompare++)
        {
            if (playerInfo[playerToCompare].getGameScore() == scoreToCheck)
            {
                playerInfo[playerToCompare].setRankingInGame(rankIndex);
            }
        }
    }
    //Assign TeamRanking

    //Total all the rankings for each team.
    for (int index = 1;  index <= 8;  index++)    team1rankTotals += playerInfo[index].getRankingInGame();
    for (int index = 9;  index <= 16; index++)    team2rankTotals += playerInfo[index].getRankingInGame();
    for (int index = 17; index <= 24; index++)    team3rankTotals += playerInfo[index].getRankingInGame();

    //Force team3 ranking to a stupidly high number if only 2 teams in the game.
    if (gameInfo.getNumberOfTeams() == 2) team3rankTotals += 32768;

    qDebug() << "-----------------------------------\nDeBrief::calculateRankings()";
    qDebug() << "team1rankTotals" << team1rankTotals;
    qDebug() << "team2rankTotals" << team2rankTotals;
    qDebug() << "team3rankTotals" << team3rankTotals;

    //Compare the rankings
    QMultiMap<int, int> teamRankTable;
    teamRankTable.insert(team1rankTotals, 1);
    teamRankTable.insert(team2rankTotals, 2);
    teamRankTable.insert(team3rankTotals, 3);

    //extract the ranking order and save to playerInfo.
    QMapIterator<int, int> teamRankIterator(teamRankTable);

        for (int index = 1; index <=3; index++)
        {
            teamRankIterator.next();
            if      (teamRankIterator.value() == 1)  gameInfo.setTeam1rank(index);
            else if (teamRankIterator.value() == 2)  gameInfo.setTeam2rank(index);
            else if (teamRankIterator.value() == 3)  gameInfo.setTeam3rank(index);
        }
        qDebug() << "DeBrief::calculateRankings()" << teamRankTable.values();
}

bool DeBrief::getIsPlayerDeBriefed() const
{
    return isPlayerDeBriefed;
}

void DeBrief::setIsPlayerDeBriefed(bool value)
{
    isPlayerDeBriefed = value;
}

void DeBrief::prepareNewPlayerToDebrief(int playerToDebrief)
{
    //qDebug() << "\n____________\nDeBrief::prepareNewPlayerToDebrief()";
    emit SendToHGWlistWidget("Preparing to debrief next player");
    currentPlayer = playerToDebrief;

    isPlayerDeBriefed           = false;
    isTeam1TagReportDue         = false;
    isTeam2TagReportDue         = false;
    isTeam3TagReportDue         = false;
    isTeam1TagReportReceived    = false;
    isTeam2TagReportReceived    = false;
    isTeam3TagReportReceived    = false;
    isSummaryTagReportReceived  = false;
    timeOutCount                = 0;
    deBriefMessageType = REQUEST_ALL_DEBRIEF_BITS;
}

bool DeBrief::checkIfPlayerIsDebriefed()
{
    //qDebug() << "DeBrief::checkIfPlayerIsDebriefed()";
    bool isTeam1ok      = false;
    bool isTeam2ok      = false;
    bool isTeam3ok      = false;
    bool result         = false;

    deBriefMessageType = 0;

    if(isSummaryTagReportReceived)  // We have received a summary :-)
    {
        //qDebug() << "DeBrief::checkIfPlayerIsDebriefed() - Summary received";
        // Set the isTeam?ok if the data has been received OR is not expected
        if(isTeam1TagReportReceived == true || isTeam1TagReportDue == false) isTeam1ok = true;
        if(isTeam2TagReportReceived == true || isTeam2TagReportDue == false) isTeam2ok = true;
        if(isTeam3TagReportReceived == true || isTeam3TagReportDue == false) isTeam3ok = true;

        // if all 3 teams are ok then player is debriefed
        if(isTeam1ok && isTeam2ok && isTeam3ok)
        {
            setIsPlayerDeBriefed(true);
            deBriefMessageType = REQUEST_ALL_DEBRIEF_BITS;
            qDebug() << "---------------------" << endl;
			lttoComms->setDontAnnounceGame(false);
            result = true;
        }
    }

    //calculate the debrief message based on what we have received
    if(isSummaryTagReportReceived == false) deBriefMessageType += REQUEST_TAG_SUMMARY_BIT;
    if(isTeam1TagReportReceived == false)   deBriefMessageType += REQUEST_TEAM1_REPORT_BIT;
    if(isTeam2TagReportReceived == false)   deBriefMessageType += REQUEST_TEAM2_REPORT_BIT;
    if(isTeam3TagReportReceived == false)   deBriefMessageType += REQUEST_TEAM3_REPORT_BIT;

    return result;
}
