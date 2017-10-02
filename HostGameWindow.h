#ifndef HOSTGAMEWINDOW_H
#define HOSTGAMEWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QSoundEffect>
#include <QPointer>
#include "Hosting.h"
#include "ReHostTagger.h"
#include "DeBrief.h"

namespace Ui {
class HostGameWindow;
}

class HostGameWindow : public QDialog
{
    Q_OBJECT

public:
    explicit HostGameWindow(QWidget *parent = 0);
    ~HostGameWindow();

    void hideEvent(QHideEvent *hideEvent);  //Used to disable SerialPort when the window is hidden instead of closed

    bool getIsThisPlayerHosted(int playerNumber) const;
    void setIsThisPlayerHosted(int playerNumber, bool value);

    int  getCurrentPlayer() const;
    void setCurrentPlayer(int value);

    bool resetPlayersForNewGame();
    void changeMode(int mode);

public slots:

    void AssignPlayer(int Game, int Tagger, int Flags, bool isLtar);   //Connects to RequestJoinGame signal

    //void EnhancedAssignPlayer(int Game, int Tagger, int Flags, int TaggerInfo, int SmartDeviceInfo); //Connects to EnhancedRequestJoinGame signal

    void AddPlayerToGame(int Game, int Tagger, bool isLtar);           //Connects to AcknowledgePlayerAssignment signal

    void AddSerialPortToListWidget(QString value);

    void InsertToListWidget(QString lineText);

private slots:

    void announceGame();

    void sendCountDown();

    void updateGameTimeRemaining();

    void deBriefTaggers();

    void sendAssignFailedMessage();

    void on_btn_Cancel_clicked();

    void on_btn_StartGame_clicked();

    void on_btn_SkipPlayer_clicked();

    void on_btn_Rehost_clicked();

    void on_btn_FailSend_clicked();

    void TaggerReHost();

    void BeaconSignal();

    void on_btn_ShowListWidget_clicked();

    void on_btn_DeBugSendTag_clicked();

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
    QPointer<ReHostTagger>  reHostTagger;
    QSoundEffect            *sound_Hosting;
    QSoundEffect            *sound_Countdown;
    QSoundEffect            *sound_HostingMissedReply;
    QSoundEffect            *sound_GoodLuck;
    QSoundEffect            *sound_PlayerAdded;
    DeBrief                 *deBrief;


    //variables
    bool    useLazerSwarm;                      // determines whether to use Lazerswarm coding or my own coding.
    bool    isThisPlayerHosted[25];             // an array that defines which players have joined the game.
    int     currentPlayer;                      // the current player to host.
    bool    noMorePlayers;
    int     countDownTimeRemaining;
    bool    expectingAckPlayerAssignment;
    int     remainingGameTime;
    bool    closingWindow;
    bool    sendingCommsActive;
    bool    rehostingActive;
    int     assignPlayerFailCount;
    bool    dontAnnounceFailedSignal;

    //methods
    void    hostCurrentPlayer();
    void    assignPlayerFailed();
    int     calculatePlayerTeam5bits(int requestedTeam);
    void    endGame();
};

#endif // HOSTGAMEWINDOW_H
