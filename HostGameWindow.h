#ifndef HOSTGAMEWINDOW_H
#define HOSTGAMEWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QSoundEffect>
#include "ReHostTagger.h"

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

    int getCurrentPlayer() const;
    void setCurrentPlayer(int value);

    bool resetPlayersForNewGame();

    int GetRandomNumber(int min, int max);

public slots:

    void AssignPlayer(int Game, int Tagger, int Flags);   //Connects to RequestJoinGame signal

    void AddPlayerToGame(int Game, int Tagger);           //Connects to AcknowledgePlayerAssignment signal

    void AddSerialPortToListWidget(QString value);

    void SetAnnounceTimerBlock(bool state);

    void InsertToListWidget(QString lineText);

private slots:

    void announceGame();

    void sendCountDown();

    void updateGameTimeRemaining();

    void deBriefTaggers();

     void sendAssignFailed();

    void on_btn_Cancel_clicked();

    void on_btn_Start_clicked();

    void on_btn_StartGame_clicked();

    void on_btn_SkipPlayer_clicked();

    void on_btn_Rehost_clicked();

    void on_btn_FailSend_clicked();

private:
    Ui::HostGameWindow  *ui;
    QTimer              *timerAnnounce;
    QTimer              *timerCountDown;
    QTimer              *timerDeBrief;
    QTimer              *timerAssignFailed;
    QTimer              *timerGameTimeRemaining;
    ReHostTagger        *reHostTagger;
    QSoundEffect        *sound_Hosting;
    QSoundEffect        *sound_Countdown;
    QSoundEffect        *sound_HostingMissedReply;
    QSoundEffect        *sound_GoodLuck;

    bool    bluetoothActive;
    bool    serialUSBactive;
    bool    wiFiActive;
    bool    useLazerSwarm;
    bool    isThisPlayerHosted[25];
    int     currentPlayer;
    bool    noMorePlayers;
    int     countDownTimeRemaining;
    bool    expectingAckPlayerAssignment;
    int     remainingGameTime;
    bool    closingWindow;
    bool    sendingCommsActive;

    void    hostCurrentPlayer();
    void    assignPlayerFailed();
    int     calculatePlayerTeam5bits(int requestedTeam);
    int     ConvertDecToBCD(int dec);
    int     ConvertBCDtoDec(int bcd);

    bool    assignSpies();
};

#endif // HOSTGAMEWINDOW_H








//void HostGameWindow::on_btn_TestAnnounce_clicked()
//{

//    const int Game = 4;

//    ////--------------------------------------------------
//    //  Host 2TeamRespawn Roonka Game (captured packets from LTTO host)     - WORKS - Tagger screen shows 2TRS
//    QString hostedGameType         = "0C";
//    QString hostedGameID           = "42";
//    QString hostedGameTime         = "15";
//    QString hostedTagsAvailable    = "99";
//    QString hostedReloadsAvailable = "FF";
//    QString hostedShieldTime       = "60";
//    QString hostedMegaTags         = "15";
//    QString hostedPackedFlags1     = "A8";
//    QString hostedPackedFlags2     = "32";
//    QString name1                  = "32";
//    QString name2                  = "54";
//    QString name3                  = "52";
//    QString name4                  = "53";
//    QString checkSum               = "00";

//    if      (Game == 0) qDebug() << endl << "Sending Roonka Game";

//    else if (Game ==1)
//    {
//        qDebug() << endl << "Sending Patent Game";
//        ////--------------------------------------------------
//        //  Game info from Patent                                           - WORKS - Tagger screen shows 2ZON
//        hostedGameType         = "0C";  //HEX
//        hostedGameID           = "2C";  //HEX
//        hostedGameTime         = "15";  //DEC
//        hostedTagsAvailable    = "50";  //DEC
//        hostedReloadsAvailable = "FF";  //DEC
//        hostedShieldTime       = "45";  //DEC
//        hostedMegaTags         = "12";  //DEC
//        hostedPackedFlags1     = "28";  //HEX
//        hostedPackedFlags2     = "A2";  //HEX
//        name1                  = "32";  //HEX
//        name2                  = "5A";  //HEX
//        name3                  = "4F";  //HEX
//        name4                  = "4E";  //HEX
//        checkSum               = "ZZ";
//        //checkSum               = "E6";  //HEX
//    }

//    else if (Game ==2)
//    {
//        qDebug() << endl << "Sending LazerSwarm Game";
//        ////--------------------------------------------------
//        //  Game info from Patent                                           - WORKS - Tagger responds with RequestJoinGame
//        hostedGameType         = "02";  //HEX
//        hostedGameID           = "7C";  //HEX
//        hostedGameTime         = "10";  //DEC
//        hostedTagsAvailable    = "10";  //DEC
//        hostedReloadsAvailable = "FF";  //DEC
//        hostedShieldTime       = "15";  //DEC
//        hostedMegaTags         = "10";  //DEC
//        hostedPackedFlags1     = "20";  //HEX
//        hostedPackedFlags2     = "01";  //HEX
//        name1                  = "00";  //HEX
//        name2                  = "00";  //HEX
//        name3                  = "00";  //HEX
//        name4                  = "00";  //HEX
//        checkSum               = "ZZ";
//        //checkSum               = "E3";  //HEX
//    }

//    else if (Game ==3)
//    {
//        qDebug() << endl << "Sending LTTO default Game";
//        ////--------------------------------------------------
//        //  Game info from Patent                                               - WORKS - Tagger responds with RequestJoinGame
//        hostedGameType         = "02";  //HEX
//        hostedGameID           = "A1";  //HEX
//        hostedGameTime         = "10";  //DEC
//        hostedTagsAvailable    = "10";  //DEC
//        hostedReloadsAvailable = "FF";  //DEC
//        hostedShieldTime       = "15";  //DEC
//        hostedMegaTags         = "10";  //DEC
//        hostedPackedFlags1     = "20";  //HEX
//        hostedPackedFlags2     = "01";  //HEX
//        name1                  = "00";  //HEX
//        name2                  = "00";  //HEX
//        name3                  = "00";  //HEX
//        name4                  = "00";  //HEX
//        checkSum               = "ZZ";
//        //checkSum               = "08";  //HEX
//    }

//    else if (Game ==4)
//    {
//        qDebug() << endl << "Sending Copy of saved LazerSwarm";
//        ////--------------------------------------------------
//        //  Game info from Patent                                               - NoGo !
//        hostedGameType         = "02";
//        hostedGameID           = "3B";
//        hostedGameTime         = "10";
//        hostedTagsAvailable    = "0A";
//        hostedReloadsAvailable = "FF";
//        hostedShieldTime       = "0F";  //DEC
//        hostedMegaTags         = "0A";  //DEC
//        hostedPackedFlags1     = "14";  //HEX
//        hostedPackedFlags2     = "01";  //HEX
//        name1                  = "00";  //HEX
//        name2                  = "00";  //HEX
//        name3                  = "00";  //HEX
//        name4                  = "00";  //HEX
//        checkSum               = "ZZ";
//        //checkSum               = "08";  //HEX
//    }


//    serialComms.sendPacket('P', hostedGameType);
//    serialComms.sendPacket('D', hostedGameID);
//    serialComms.sendPacket('D', hostedGameTime);
//    serialComms.sendPacket('D', hostedTagsAvailable);
//    serialComms.sendPacket('D', hostedReloadsAvailable);
//    serialComms.sendPacket('D', hostedShieldTime);
//    serialComms.sendPacket('D', hostedMegaTags);
//    serialComms.sendPacket('D', hostedPackedFlags1);
//    serialComms.sendPacket('D', hostedPackedFlags2);
//    if (name1 != "00")
//    {
//        serialComms.sendPacket('D', name1);
//        serialComms.sendPacket('D', name2);
//        serialComms.sendPacket('D', name3);
//        serialComms.sendPacket('D', name4);
//    }
//    serialComms.sendPacket('C');
//}
