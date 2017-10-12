#ifndef DEBRIEF_H
#define DEBRIEF_H

#include <QObject>
#include <QTimer>

class DeBrief : public QObject
{
    Q_OBJECT
public:
    explicit DeBrief(QObject *parent = nullptr);

    bool getIsPlayerDeBriefed() const;
    void setIsPlayerDeBriefed(bool value);

signals:

public slots:
    void    RequestTagReports(int playerToInterogate);
    void    ReceiveTagSummary(int game, int teamAndPlayer, int tagsTaken, int survivedMinutes, int survivedSeconds, int zoneTimeMinutes, int zoneTimeSeconds, int flags);
    void    Team1TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
    void    Team2TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);
    void    Team3TagReportReceived(int game, int teamAndPlayer, int tagsP1, int tagsP2, int tagsP3, int tagsP4, int tagsP5, int tagsP6, int tagsP7, int tagsP8);

    void    sendRankReport();


private:

    int     maxTeamNum;                 // the highest team number in the game.
    int     deBriefTeam;                // current Team (incremented when all players in team are debriefed).
    int     deBriefPlayer;              // the player we are debriefing, 0-8 and related to deBriefTeam.
    int     currentPlayer;              // the player index for playerInfo[] - 1 thru 24
    int     deBriefMessageType;         // what message are we asking for (TAG_SUMMARY, TEAM_1_TAG_REPORT, TEAM_2_TAG_REPORT, TEAM_3_TAG_REPORT)
    bool    isPlayerDeBriefed;          // set true when player has replied to all mesage requests.

    bool    isTeam1TagReportDue;        // this player has been tagged by someone in Team 1 and needs to tell us about it.
    bool    isTeam2TagReportDue;        // this player has been tagged by someone in Team 2 and needs to tell us about it.
    bool    isTeam3TagReportDue;        // this player has been tagged by someone in Team 3 and needs to tell us about it.

    bool    decodeTeamAndPlayer(int teamAndPlayer);        // translates TeamAndPlayer Byte into player and team, then checks it matches currentPlayer;
};

#endif // DEBRIEF_H
