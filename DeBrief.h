#ifndef DEBRIEF_H
#define DEBRIEF_H

#include <QObject>
#include <QTimer>

class DeBrief : public QObject
{
    Q_OBJECT
public:
    explicit DeBrief(QObject *parent = nullptr);

signals:

public slots:
    void    RequestTagReports(int playerToInterogate);
    void    ReceiveTagSummary(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags);

private:

    int     maxTeamNum;                 // the highest team number in the game.
    int     deBriefTeam;                // current Team (incremented when all players in team are debriefed).
    int     deBriefPlayer;              // the player we are debriefing, 0-8 and related to deBriefTeam.
    int     currentPlayer;              // the player index for playerInfo[] - 1 thru 24
    int     deBriefMessageType;         // what message are we asking for (TAG_SUMMARY, TEAM_1_TAG_REPORT, TEAM_2_TAG_REPORT, TEAM_3_TAG_REPORT)

    bool    isTeam1TagReportDue;        // this player has been tagged by someone in Team 1 and needs to tell us about it.
    bool    isTeam2TagReportDue;        // this player has been tagged by someone in Team 2 and needs to tell us about it.
    bool    isTeam3TagReportDue;        // this player has been tagged by someone in Team 3 and needs to tell us about it.
};

#endif // DEBRIEF_H
