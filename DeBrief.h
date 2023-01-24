#ifndef DEBRIEF_H
#define DEBRIEF_H

#include <QObject>
#include <QTimer>
#include "LttoComms.h"

class DeBrief : public QObject
{
    Q_OBJECT

private:
	explicit    DeBrief(QObject *parent = nullptr);
	static		DeBrief *instance;

public:
	static		DeBrief *getInstance();

	bool		getIsPlayerDeBriefed() const;
	void		setIsPlayerDeBriefed(bool value);
	bool		getIsPlayerDebriefing() const;
	void		setIsPlayerDebriefing(bool value);

	void		prepareNewPlayerToDebrief(int playerToDebrief);
	bool		checkIfPlayerIsDebriefed();
	bool		checkIfAllPlayersAreDebriefed();
	void		RequestTagReports();
	void		calculateScores();                      // calculate the scores using POINTS_FOR_XXX in Defines.h
	void		calculateRankings();                    // assign rankings to each player



	int			getDeBriefMessageType() const;

signals:
	void		SendToHGWlistWidget(QString message);

public slots:

	void		ReceiveTagSummary(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags);
	void		Team1TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
	void		Team2TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
	void		Team3TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
	void		sendRankReport();


private:

	LttoComms	*lttoComms;
	int			maxTeamNum;                 // the highest team number in the game.
	int			deBriefTeam;                // current Team (incremented when all players in team are debriefed).
	int			deBriefPlayer;              // the player we are debriefing, 0-8 and related to deBriefTeam.
	int			currentPlayer;              // the player index for playerInfo[] - 1 thru 24
	int			deBriefMessageType;         // what message are we asking for (TAG_SUMMARY, TEAM_1_TAG_REPORT, TEAM_2_TAG_REPORT, TEAM_3_TAG_REPORT)
	bool		isPlayerDeBriefed;          // set true when player has replied to all mesage requests.
	bool		isPlayerDebriefing;			// set true when a Tag Summary is received. Used for non-linear debriefing.
	bool		awaitingDeBriefPackets;     // Set true when a RequestDebrief message is sent. Cancelled after 3 seconds.
	int			timeOutCount;               // Used to count 3 seconds for above.....
	int			currentTeamAndPlayerByte;	// The TeamAndPlayerByte of the current player. Generating in RequesTeamTagSummary and used in isPlayerDebriefed for LTAR mode.

	bool		isTeam1TagReportDue;        // this player has been tagged by someone in Team 1 and needs to tell us about it.
	bool		isTeam2TagReportDue;        // this player has been tagged by someone in Team 2 and needs to tell us about it.
	bool		isTeam3TagReportDue;        // this player has been tagged by someone in Team 3 and needs to tell us about it.
	bool		isTeam1TagReportReceived;   // have we recevied a valid Team 1 tag report.
	bool		isTeam2TagReportReceived;   // have we recevied a valid Team 2 tag report.
	bool		isTeam3TagReportReceived;   // have we recevied a valid Team 3 tag report.
	bool		isSummaryTagReportReceived; // have we recevied a valid Suammary Tag report.

	bool		decodeTeamAndPlayer(int teamAndPlayer); // translates TeamAndPlayer Byte into player and team, then checks it matches currentPlayer;
};

#endif // DEBRIEF_H
