#ifndef HOSTGAMEWINDOW_H
#define HOSTGAMEWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QSoundEffect>
#include <QPointer>
#include "Hosting.h"
#include "ReHostTagger.h"
#include "DeBrief.h"
#include "ScoresWindow.h"
#include "LttoComms.h"

namespace Ui {
class HostGameWindow;
}

class HostGameWindow : public QDialog
{
    Q_OBJECT

public:
	explicit HostGameWindow(QWidget *parent = nullptr);
    ~HostGameWindow();

    void hideEvent(QHideEvent *hideEvent);  //Used to disable SerialPort when the window is hidden instead of closed

    bool getIsThisPlayerHosted(int playerNumber) const;
    void setIsThisPlayerHosted(int playerNumber, bool value);

    int  getCurrentPlayer() const;
    void setCurrentPlayer(int value);

    bool resetPlayersForNewGame();
    void changeMode(int mode);

    bool getAwaitingDeBriefPackets() const;
    void setAwaitingDeBriefPackets(bool value);

public slots:

    void AssignPlayer(int Game, int Tagger, int Flags, bool isLtar);   //Connects to RequestJoinGame signal

    //void EnhancedAssignPlayer(int Game, int Tagger, int Flags, int TaggerInfo, int SmartDeviceInfo); //Connects to EnhancedRequestJoinGame signal

    void AddPlayerToGame(int Game, int Tagger, bool isLtar);           //Connects to AcknowledgePlayerAssignment signal

	//void LtarPlayerJoinedGame(int Game, int Tagger, int teamAndPlayer);	//Connects to LtarAckPlayerAssignment signal

	void InsertToListWidget(QString lineText);

signals:

	void closingHostGameWindow();

private slots:

    void announceGame();

    void sendCountDown();

    void updateGameTimeRemaining();

    void deBriefTaggers();

	void assignPlayerFailed();

    void sendAssignFailedMessage();

    void on_btn_Cancel_clicked();

    void on_btn_StartGame_clicked();

    void on_btn_SkipPlayer_clicked();

    void on_btn_Rehost_clicked();

    void TaggerReHost();

    void BeaconSignal();

	void closeHostGameWindow();

	void on_showLog_clicked();

	void UpdateBatteryDisplay(QString volts);

	void resetScores();

private:
    //pointers
    Ui::HostGameWindow      *ui;
    QTimer                  *timerAnnounce;
    QTimer                  *timerCountDown;
    QTimer                  *timerDeBrief;
    QTimer                  *timerAssignFailed;
    QTimer                  *timerGameTimeRemaining;
    QTimer                  *timerReHost;
	QTimer                  *timerBeacon;
	QTimer					*timerAckNotReceived;
    QPointer<ReHostTagger>  reHostTagger;
    QPointer<ScoresWindow>  scoresWindow;
	QPointer<DeBrief>       deBrief;
    QSoundEffect            *sound_Hosting;
    QSoundEffect            *sound_Countdown;
    QSoundEffect            *sound_HostingMissedReply;
    QSoundEffect            *sound_GoodLuck;
    QSoundEffect            *sound_PlayerAdded;
	LttoComms				*lttoComms;
	Hosting					*host;

    //variables
	//bool    useLazerSwarm;                    // determines whether to use Lazerswarm comms or my own comms method.
    bool    isThisPlayerHosted[25];             // an array that defines which players have joined the game.
    int     currentPlayer;                      // the current player to host.
	int		nextPlayer;							// the next player to host
    bool    noMorePlayers;
    int     countDownTimeRemaining;
    bool    expectingAckPlayerAssignment;
	bool	expectingLtarAssignSuccess;
    int     remainingGameTime;
    bool    closingWindow;
    bool    sendingCommsActive;
    bool    rehostingActive;
    int     assignPlayerFailCount;
	int		beaconType;
    //bool    dontAnnounceFailedSignal;
    //bool    firstPassThisPlayer;

    //methods
	void	setPromptText(QString text);
	void	setNextPlayerText(QString text);
    void    hostCurrentPlayer();
    int     calculatePlayerTeam5bits(int requestedTeam);
    void    endGame();
};

#endif // HOSTGAMEWINDOW_H
