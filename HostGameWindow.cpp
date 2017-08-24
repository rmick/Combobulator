#include "HostGameWindow.h"
#include "ui_HostGameWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include "Defines.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "TCPComms.h"

#include <QInputDialog>

int playerDebugNum = 0;

HostGameWindow::HostGameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostGameWindow),
    timerAnnounce(NULL),
    timerCountDown(NULL),
    timerDeBrief(NULL),
    timerAssignFailed(NULL),
    timerGameTimeRemaining(NULL),
    timerReHost(NULL),
    timerBeacon(NULL),
    sound_Hosting(NULL),
    sound_Countdown(NULL),
    sound_HostingMissedReply(NULL),
    sound_GoodLuck(NULL),
    sound_PlayerAdded(NULL)
{
    ui->setupUi(this);
    changeMode(HOST_MODE);

    timerAnnounce           = new QTimer(this);
    timerCountDown          = new QTimer(this);
    timerDeBrief            = new QTimer(this);
    timerAssignFailed       = new QTimer(this);
    timerGameTimeRemaining  = new QTimer(this);
    timerReHost             = new QTimer(this);
    timerBeacon             = new QTimer(this);

    connect(timerAnnounce,          SIGNAL(timeout() ),             this, SLOT(announceGame())            );
    connect(timerCountDown,         SIGNAL(timeout() ),             this, SLOT(sendCountDown())           );
    connect(timerDeBrief,           SIGNAL(timeout() ),             this, SLOT(deBriefTaggers())          );
    connect(timerAssignFailed,      SIGNAL(timeout() ),             this, SLOT(sendAssignFailedMessage())        );
    connect(timerGameTimeRemaining, SIGNAL(timeout() ),             this, SLOT(updateGameTimeRemaining()) );
    connect(timerReHost,            SIGNAL(timeout() ),             this, SLOT(TaggerReHost()) );
    connect(timerBeacon,            SIGNAL(timeout() ),             this, SLOT(BeaconSignal()) );
    connect(&lttoComms,      SIGNAL(RequestJoinGame(int,int,int) ), this, SLOT(AssignPlayer(int,int,int)) );
    connect(&lttoComms,      SIGNAL(AckPlayerAssignment(int,int) ), this, SLOT(AddPlayerToGame(int,int))  );
    connect(&serialUSBcomms, SIGNAL(SerialPortFound(QString)),      this, SLOT(AddSerialPortToListWidget(QString)) );

    timerAnnounce->start(HOST_TIMER_MSEC);

    for (int x = 0; x<25;x++) isThisPlayerHosted[x] = false;
    currentPlayer = 1;
    noMorePlayers = false;
    remainingGameTime = gameInfo.getGameLength()*60;
    playerDebugNum = 0;
    closingWindow = false;
    sendingCommsActive = false;
    rehostingActive = false;
    countDownTimeRemaining = DEFAULT_COUNTDOWN_TIME;
    ui->btn_Cancel->setText("Cancel");
    ui->btn_Rehost->setEnabled(false);

    //Init all the sound effects.
    sound_Hosting            = new QSoundEffect(this);
    sound_Countdown          = new QSoundEffect(this);
    sound_HostingMissedReply = new QSoundEffect(this);
    sound_GoodLuck           = new QSoundEffect(this);
    sound_PlayerAdded        = new QSoundEffect(this);
    sound_Hosting           ->setSource(QUrl::fromLocalFile(":/resources/audio/hosting-listening.wav"));
    sound_HostingMissedReply->setSource(QUrl::fromLocalFile(":/resources/audio/hosting-problem.wav"));
    sound_Countdown         ->setSource(QUrl::fromLocalFile(":/resources/audio/countdown.wav"));
    sound_GoodLuck          ->setSource(QUrl::fromLocalFile(":/resources/audio/good-luck.wav"));
    sound_PlayerAdded       ->setSource(QUrl::fromLocalFile(":/resources/audio/hosting-join-complete.wav"));
    sound_Countdown         ->setLoopCount(5);

    //TCPComms tcpComms;
    if(serialUSBcomms.getIsUSBinitialised() == false)
    {
        serialUSBcomms.setIsUSBinitialised(true);
        serialUSBcomms.setUpSerialPort();
        serialUSBcomms.initialiseUSBsignalsAndSlots();
        //tcpComms.initialiseTCPsignalsAndSlots();
    }
}

HostGameWindow::~HostGameWindow()
{
    timerAnnounce->stop();
    timerCountDown->stop();
    timerDeBrief->stop();
    timerAssignFailed->stop();
    timerGameTimeRemaining->stop();
    timerReHost->stop();
    delete ui;
}

void HostGameWindow::hideEvent(QHideEvent *)
{
    closingWindow = true;
    timerAnnounce->stop();
    timerCountDown->stop();
    timerDeBrief->stop();
    timerAssignFailed->stop();
    timerGameTimeRemaining->stop();
    timerReHost->stop();
}

void HostGameWindow::on_btn_Cancel_clicked()
{
    closingWindow = true;
    ui->btn_Cancel->setEnabled(false);
    ui->label->setText("Cancelling......");
    timerAnnounce->stop();
    timerCountDown->stop();
    timerDeBrief->stop();
    timerAssignFailed->stop();
    timerGameTimeRemaining->stop();
    timerReHost->stop();
    if (sendingCommsActive == false) deleteLater();     //If this is true then the deleteLater is triggered at the end of hostCurrentPlayer(), to stop the app crashing.
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

void HostGameWindow::announceGame()
{
    sendingCommsActive = true;                                                              // Used to stop the class being destructed, if the Timer triggered.

    //Set Base Station LED status
    if (lttoComms.getTcpCommsConnected() == false)
    {
        ui->led_Status->setStyleSheet("background-color: yellow; border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey;");
    }
    else
    {
        ui->led_Status->setStyleSheet("background-color: rgb(0,255,50); border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey;");
    }


    if (currentPlayer != 0)                                                                 // Player 0 is the dummy player
    {
        while (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false) currentPlayer++;  // Find next player

        if (currentPlayer < 25)     ui->label->setText("Prepare to Host : " + playerInfo[currentPlayer].getPlayerName());
    }
    if (currentPlayer > 16 && gameInfo.getNumberOfTeams() == 2) currentPlayer = 25;         // Ignore players in Team 3
    if (currentPlayer >24)                                                                  // No more players to add
    {
        ui->label->setText("All players are hosted, press START ");
        ui->btn_SkipPlayer->setVisible(false);
        ui->btn_StartGame->setEnabled(true);
        currentPlayer = 0;                                                                  //Transmits global game data to keep guns sync'd
    }

    hostCurrentPlayer();
}


void HostGameWindow::hostCurrentPlayer()
{
    qDebug() << "HostGameWindow::hostCurrentPlayer() = " << currentPlayer;

    if(lttoComms.getDontAnnounceGame() != false)                                             // We are receiving data, so dont announce.
    {
        lttoComms.setMissedAnnounceCount(lttoComms.getMissedAnnounceCount()+1);             // Increment counter
        qDebug() << "HostGameWindow::hostCurrentPlayer() - Did not Announce, due to Rx arrival. Count = " << lttoComms.getMissedAnnounceCount();
        if (lttoComms.getMissedAnnounceCount() < 3) return;

        if (expectingAckPlayerAssignment == true)                                           // We appear to have missed the AckPlayerAssignment message.
        {
            qDebug() << "HostGameWindow::hostCurrentPlayer() - assignPlayerFailed !!"  ;
            assignPlayerFailed();                                                           // Call the AssignPlayerFailed routine.
        }
        else
        {
            lttoComms.setDontAnnounceGame(false);                                           // Start Announcing, as something went wrong.
            //lttoComms.setMissedAnnounceCount(0);                                            // Reset the counter.
        }
    }

    sound_Hosting->play();

    //Change TeamTags if required for Spies.
    bool wereCurrentPlayerTeamTagsActive = playerInfo[currentPlayer].getTeamTags();
    if(playerInfo[currentPlayer].getSpyNumber() != 0)   //Player is a spy
    {
        if(gameInfo.getIsSpiesTeamTagActive()) playerInfo[currentPlayer].setTeamTags(true);
    }

    if (lttoComms.getTcpCommsConnected() == true || serialUSBcomms.getSerialCommsConnected() == true)
    {
        InsertToListWidget("Announce Game : Player " + QString::number(currentPlayer));
    }
    else
    {
        InsertToListWidget("Trying to connect to Base Station");
        ui->label->setText("Connecting to Base Station......");
    }

    // Set all the variables.
    int gameTime;
    //  Set gameTime to gameLength or TimeRemaining
    if(rehostingActive) gameTime = (remainingGameTime/60)+1;
    else                gameTime = gameInfo.getGameLength();

    //  Set the Reloads flag to match handicapped values
    if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads())  <  100)
         playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, true );
    else playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, false);

//    else if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads())  == 100)
//    {
//        playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, false);
//    }

    //  Set the MegaTags flag to match handicapped values
    if      (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()) < 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_MEGAS_FLAG,   true);
    }
    else if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()) == 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_MEGAS_FLAG,   false);
    }

    // Send the message.
    lttoComms.sendPacket(PACKET, gameInfo.getGameType()    );
    lttoComms.sendPacket(DATA,   gameInfo.getGameID()      );
    lttoComms.sendPacket(DATA,   gameTime,              BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getHealthTags()), BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads()),    BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getShieldTime()), BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()),   BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].getPackedFlags1()   );
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].getPackedFlags2()   );
    //  Add a name for Custom Game Types, otherwise skip.
    if (gameInfo.getGameType() == 0x0C)
    {
        lttoComms.sendPacket(DATA, gameInfo.getNameChar1() );
        lttoComms.sendPacket(DATA, gameInfo.getNameChar2() );
        lttoComms.sendPacket(DATA, gameInfo.getNameChar3() );
        lttoComms.sendPacket(DATA, gameInfo.getNameChar4() );
    }
    //send the Checksum
    lttoComms.sendPacket(CHECKSUM);

    //If the current Player did not have TeamTags active, turn them off again. (altered if they are a Spy)
    if(wereCurrentPlayerTeamTagsActive == false) playerInfo[currentPlayer].setTeamTags(false);

    //TODO:DEBUG
    if(playerDebugNum != currentPlayer)
    {
        playerDebugNum = currentPlayer;
        QString playerDebugData;
        playerDebugData = "Debug...Player #" + QString::number(currentPlayer)
                            + ", Tags=" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getHealthTags() ))
                            + ", Reloads=" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads() ))
                            + ", Shields=" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getShieldTime() ))
                            + ", Megas=" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags() ))
                            + ", MedicMode:" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMedicMode() ))
                            + ", SlowTags:" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getSlowTags() ))
                            + ", TeamTags:" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getTeamTags() ))
                            + ", Spy#:" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getSpyNumber() ));
        InsertToListWidget(playerDebugData);
        InsertToListWidget("Flag1 = " + QString::number(playerInfo[currentPlayer].getPackedFlags1()) + " Flag2 = " + QString::number(playerInfo[currentPlayer].getPackedFlags2()));

    }

    sendingCommsActive = false;
    if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
    return;
}


void HostGameWindow::AssignPlayer(int Game, int Tagger, int Flags)

{
    lttoComms.setDontAnnounceGame(true);

    //Check if the gameID from the tagger is correct.
    if(gameInfo.getGameID() != Game)
    {
        qDebug() << "HostGameWindow::AssignPlayer - GameID mismatch !!!!!!      ERROR";
        lttoComms.setDontAnnounceGame(false);
        return;
    }

    int preferedTeam = Flags & 3;  //b00000011;       //This is the 2 LSB of the flags byte.

    if(isThisPlayerHosted[currentPlayer] == false)
    {
        playerInfo[currentPlayer].setTaggerID(Tagger);

        lttoComms.sendPacket(PACKET,  ASSIGN_PLAYER);
        lttoComms.sendPacket(DATA,    Game);
        lttoComms.sendPacket(DATA,    Tagger);
        lttoComms.sendPacket(DATA,    calculatePlayerTeam5bits(preferedTeam) );
        lttoComms.sendPacket(CHECKSUM);

        expectingAckPlayerAssignment = true;

        InsertToListWidget ("   AssignPlayer()" + QString::number(currentPlayer) + ", Tagger ID:" + QString::number(Tagger) );
        qDebug() << "\n\tHostGameWindow::AssignPlayer() " << currentPlayer << Game << Tagger, calculatePlayerTeam5bits(Flags);
        if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
    }
}

void HostGameWindow::AddPlayerToGame(int Game, int Tagger)
{
    sound_PlayerAdded->play();
    qDebug() << "\tHostGameWindow::AddPlayerToGame()" << currentPlayer << endl;

    if(gameInfo.getGameID() != Game || playerInfo[currentPlayer].getTaggerID() != Tagger)
    {
        qDebug() << "HostGameWindow::AddPlayerToGame() - GameID or TaggerID mismatched !!!!!!   ERROR";
        return;
    }
    InsertToListWidget("   AddPlayerToGame()" + QString::number(currentPlayer));
    isThisPlayerHosted[currentPlayer] = true;
    expectingAckPlayerAssignment = false;
    timerAssignFailed->stop();

    if(rehostingActive)
    {
        timerReHost->stop();
        //gameInfo.setPlayerToReHost(0);
        ui->label->setText(playerInfo[currentPlayer].getPlayerName() + " has joined.");
        countDownTimeRemaining = (DEFAULT_COUNTDOWN_TIME + 5);
        ui->btn_StartGame->setEnabled(true);
        //ui->btn_StartGame->setText("End\nGame");
        ui->btn_Rehost->setText("Rehost Tagger");
        lttoComms.nonBlockingDelay(1000);
        timerCountDown->start(1000);    // Auto start the countdown to get the player into the game ASAP.
    }
    else
    {
        if (currentPlayer != 0) currentPlayer++;
    }
    lttoComms.setDontAnnounceGame(false);
    if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
}

void HostGameWindow::assignPlayerFailed()       //TODO: THis is not working.
{
    sound_HostingMissedReply->play();
    qDebug() << "HostGameWindow::assignPlayerFailed() - starting Timer";
    timerAssignFailed->start(500);
    assignPlayerFailCount = 0;
    lttoComms.setDontAnnounceGame(true);
    lttoComms.setDontAnnounceFailedSignal(false);
    // An Rx toAddPlayerToGame will cancel the timer.

    // create a new Timer (500mS)
    // send the message
    // increment counter
    // If counter hits 6 then rehost the player.
}

void HostGameWindow::sendAssignFailedMessage()
{
    if(dontAnnounceFailedSignal == false)
    {
        assignPlayerFailCount++;

        sound_HostingMissedReply->play();

        lttoComms.sendPacket(PACKET, ASSIGN_PLAYER_FAIL                      );
        lttoComms.sendPacket(DATA,   gameInfo.getGameID()                    );
        lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].getTaggerID() );
        lttoComms.sendPacket(CHECKSUM                                        );
         qDebug() << "HostGameWindow::sendAssignFailedMessage()  - Sending # " << assignPlayerFailCount;
    }

    if (assignPlayerFailCount == 6)   //give up and start again
    {
        isThisPlayerHosted[currentPlayer] = false;
        playerInfo[currentPlayer].setTaggerID(0);
        timerAssignFailed->stop();
        lttoComms.setDontAnnounceGame(false);
        expectingAckPlayerAssignment = false;
        assignPlayerFailCount = 0;
        qDebug() <<"HostGameWindow::sendAssignFailedMessage() - Counted to 6, I am now going away";
        if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
    }
}


int HostGameWindow::calculatePlayerTeam5bits(int requestedTeam)
{
    int assignedTeamNumber   = 0;
    int assignedPlayerNumber = 0;
    int playerTeam5bits      = 0;

    if(requestedTeam != 0)
    {
        qDebug() << "HostGameWindow::calculatePlayerTeam5bit() - Requested Team = " << requestedTeam;
        // Do I use this or not?
        // Best not to, otherwise the announcement to Host T1, P1 might actually join T3,P1.
    }

    if (gameInfo.getNumberOfTeams() == 0)                                                       //Players 1 to 24, no teams
    {
        assignedTeamNumber = 0;
        assignedPlayerNumber = currentPlayer +7;                                                // For solo games: zero-based player number + 8
        playerTeam5bits = 8 + (currentPlayer-1);
        return playerTeam5bits;
    }
    else
    {
       //TODO: Move this to a separate function.
       //TODO: Add Kings selection (random player SAME team) to the routine.

        // Assign Spies to the next team (1>2, 2>3 or 2>1, 3>1)

        if      (currentPlayer > 0  && currentPlayer < 9)           // Team 1
        {
            if (playerInfo[currentPlayer].getSpyNumber() == 0)      assignedTeamNumber = 1;     // Not a Spy
            else
            {
                if      (gameInfo.getNumberOfTeams() == 2)          assignedTeamNumber = 2;     // Increment to Team 2
                else if (gameInfo.getNumberOfTeams() == 3)          assignedTeamNumber = 2;     // Increment to Team 2
            }
        }
        else if (currentPlayer > 8  && currentPlayer < 17)          // Team 2
        {
            if (playerInfo[currentPlayer].getSpyNumber() == 0)      assignedTeamNumber = 2;     // Not a Spy
            else
            {
                if      (gameInfo.getNumberOfTeams() == 2)          assignedTeamNumber = 1;     //Decrement to Team 2
                else if (gameInfo.getNumberOfTeams() == 3)          assignedTeamNumber = 3;     //Increment to Team 3
            }
        }
        else if (currentPlayer > 16 && currentPlayer < 25)          // Team 3
        {
            if (playerInfo[currentPlayer].getSpyNumber() == 0)      assignedTeamNumber = 3;     // Not a Spy
            else
            {
                if      (gameInfo.getNumberOfTeams() == 2)          assignedTeamNumber = 1;     //Impossible to get here.
                else if (gameInfo.getNumberOfTeams() == 3)          assignedTeamNumber = 1;     //Increment/Rollover to Team 1
            }
        }
    }

    playerTeam5bits = (assignedTeamNumber) << 3;


    // Now the fun part, Swapping player numbers with the other spy.
            // eg,  T1-P3 and T2-P7 becomes...
            //      T2-P7 and T1-P3 WHILST keeping their own settings!!!
            //      This will make debrief interesting..........


    //TODO: Move this to a separate function as well.
    if (playerInfo[currentPlayer].getSpyNumber() == 0)                      // Not a Spy
    {
        //assignedPlayerNumber = currentPlayer -  1;
        if      (assignedTeamNumber == 1) assignedPlayerNumber = currentPlayer -  1;  // 0 based player number
        else if (assignedTeamNumber == 2) assignedPlayerNumber = currentPlayer -  9;  // 0 based player number
        else if (assignedTeamNumber == 3) assignedPlayerNumber = currentPlayer - 17;  // 0 based player number
    }
    else
    {
        if      (currentPlayer >0 && currentPlayer < 9)      //Team 1
        {
            for(int n = 1; n <= gameInfo.getNumberOfSpies(); n++)
            if      (playerInfo[currentPlayer].getSpyNumber() == n)
            {
                for (int x = 9; x < 17; x++)                // Swap a player from Team 2
                {
                    if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-9;
                }
            }
        }
        else if (currentPlayer >8 && currentPlayer < 17)      //Team 2
        {
            for(int n = 1; n <= gameInfo.getNumberOfSpies(); n++)
            if      (playerInfo[currentPlayer].getSpyNumber() == n)
            {
                if(gameInfo.getNumberOfTeams() == 2)
                {
                    for (int x = 1; x < 9; x++)             // Swap a player from Team 1
                    {
                        if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-1;
                    }
                }
                else
                {
                    for (int x = 17; x < 25; x++)           // Swap a player from Team 3
                    {
                        if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-17;
                    }
                }
            }
        }
        else if (currentPlayer >16 && currentPlayer < 25)      //Team 3
        {
            for(int n = 1; n <= gameInfo.getNumberOfSpies(); n++)
            if      (playerInfo[currentPlayer].getSpyNumber() == n)
            {
                for (int x = 1; x < 9; x++)             // Swap a player from Team 1
                {
                    if(playerInfo[x].getSpyNumber() == n) assignedPlayerNumber = x-1;
                }
            }
        }
    }
    playerTeam5bits += assignedPlayerNumber;
    int spyPlayerNumber = (assignedPlayerNumber + (8 * (assignedTeamNumber-1)) + 1);
    qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - Current Player = " << currentPlayer;;
    qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - Spy Player =     " << spyPlayerNumber << "\t- Team:" << assignedTeamNumber << ", Player:" << assignedPlayerNumber+1 << ", Spy# " << playerInfo[currentPlayer].getSpyNumber();
    qDebug() << "----";

    return playerTeam5bits;
}

void HostGameWindow::AddSerialPortToListWidget(QString value)
{
    QString portFound = QObject::tr("") + value;
    qDebug() << portFound;
    InsertToListWidget(portFound);
}

int HostGameWindow::getCurrentPlayer() const
{
    return currentPlayer;
}

void HostGameWindow::setCurrentPlayer(int value)
{
    currentPlayer = value;
}

bool HostGameWindow::resetPlayersForNewGame()
{
    currentPlayer = 1;
    for(int players= 0; players < 25; players++)
    {
        isThisPlayerHosted[players] = false;
    }
    noMorePlayers = false;
    return assignSpies();
}

bool HostGameWindow::getIsThisPlayerHosted(int playerNumber) const
{
    return isThisPlayerHosted[playerNumber];
}

void HostGameWindow::setIsThisPlayerHosted(int playerNumber, bool value)
{
    isThisPlayerHosted[playerNumber] = value;
}

void HostGameWindow::on_btn_StartGame_clicked()
{
    if(ui->btn_StartGame->text() == "Start\nthe\nGame")
    {
        timerAnnounce->stop();
        timerCountDown->start(1000);
        countDownTimeRemaining = gameInfo.getCountDownTime();
        ui->btn_StartGame->setEnabled(false);
    }
    else if(ui->btn_StartGame->text() == "End\nGame")
    {
        deBrief = new DeBrief(this);
        timerGameTimeRemaining->stop();
        timerReHost->stop();
        timerBeacon->stop();
        timerDeBrief->start(HOST_TIMER_MSEC);
        currentPlayer = 0;                          // Set ready for DeBriefing to search for next player
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Rehost->setEnabled(false);
        ui->btn_Cancel->setEnabled(true);
        ui->btn_SkipPlayer->setVisible(true);
        ui->label->setText("DeBriefing..... but not just yet :-)");
    }
    else
    {
        qDebug() << "HostGameWindow::on_btn_StartGame_clicked() - Not quite sure how we got here......";
        //ui->btn_StartGame->setText("Start\nthe\nGame");
    }

}

void HostGameWindow::sendCountDown()
{
    if(countDownTimeRemaining == 0)     // Start the game
    {
        sound_Countdown->stop();
        sound_GoodLuck->play();
        timerCountDown->stop();
        sendingCommsActive = false;         // Otherwise it is


        if (rehostingActive == false)               //Ignore the next section if the game is running.
        {
            timerGameTimeRemaining->start(1000);
            remainingGameTime = (lttoComms.ConvertBCDtoDec(gameInfo.getGameLength()) * 60) + 2;  // Add a couple of seconds to match the taggers, who start the clock AFTER the Good Luck message.
            InsertToListWidget("Game has started !!!");
            ui->label->setText("Game Underway !!!");
            timerBeacon->start(BEACON_TIMER_MSEC);
        }
        else
        {
             ui->label->setText("Player" + QString::number(currentPlayer) + " is back in the game");
             rehostingActive = false;
        }
        ui->btn_Cancel->setText("Close");
        ui->btn_Rehost->setVisible(true);
        ui->btn_Rehost->setEnabled(true);
        ui->btn_StartGame->setText("End\nGame");
        ui->btn_StartGame->setEnabled(true);
        ui->btn_Cancel->setEnabled(false);
        //TODO:Change the form to show the DeBrief stuff - use a State Machine?
    }
    else                                // Send the Countdown Signal
    {
        sound_Countdown->play();
        lttoComms.sendPacket(PACKET ,   COUNTDOWN);
        lttoComms.sendPacket(DATA,      gameInfo.getGameID() );
        lttoComms.sendPacket(DATA,      lttoComms.ConvertDecToBCD(countDownTimeRemaining));
        lttoComms.sendPacket(DATA, 8);
        lttoComms.sendPacket(DATA, 8);
        lttoComms.sendPacket(DATA, 8);
//        lttoComms.sendPacket(DATA, gameInfo.getPlayersInTeam(1));
//        lttoComms.sendPacket(DATA, gameInfo.getPlayersInTeam(2));
//        lttoComms.sendPacket(DATA, gameInfo.getPlayersInTeam(3));
        lttoComms.sendPacket(CHECKSUM);

        InsertToListWidget("CountDownTime = " + QString::number(countDownTimeRemaining));
        ui->label->setText("CountDownTime\n\n" + QString::number(countDownTimeRemaining));
        qDebug() << "CountDownTime = " + QString::number(countDownTimeRemaining);
        countDownTimeRemaining--;
    }
}

void HostGameWindow::updateGameTimeRemaining()
{
    if (!reHostTagger) ui->btn_Rehost->setEnabled(true);
    remainingGameTime--;
    if (remainingGameTime > (lttoComms.ConvertBCDtoDec(gameInfo.getGameLength()) * 60)) return;   // Ignore the extra 2 seconds we added to the clock.
    QString remainingMinutes = QString::number(remainingGameTime / 60);
    QString remainingSeconds = QString::number(remainingGameTime % 60);
    if (remainingMinutes.length() == 1) remainingMinutes.prepend("0");
    if (remainingSeconds.length() == 1) remainingSeconds.prepend("0");
    if (rehostingActive == false) ui->label->setText("Game Time Remaining\n\n" + remainingMinutes + ":" + remainingSeconds);
    if (remainingGameTime == 0)
    {
        timerGameTimeRemaining->stop();
        timerDeBrief->start(HOST_TIMER_MSEC);
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Rehost->setEnabled(false);
        ui->label->setText("DeBriefing..... but not just yet :-)");
    }
}

//int HostGameWindow::ConvertDecToBCD(int dec)
//{
//  if (dec == 0xFF) return dec;
//  return (int) (((dec/10) << 4) | (dec %10) );
//}

//int HostGameWindow::ConvertBCDtoDec(int bcd)
//{
//  if (bcd == 0xFF) return bcd;
//  return (int) (((bcd >> 4) & 0xF) *10) + (bcd & 0xF);
//}

void HostGameWindow::on_btn_SkipPlayer_clicked()
{
    currentPlayer++;
    lttoComms.setDontAnnounceGame(false);
}

void HostGameWindow::InsertToListWidget(QString lineText)
{
    ui->listWidget_Status->addItem(lineText);
    //if (ui->listWidget_Status->count() > 18) ui->listWidget_Status->takeItem(0);
    ui->listWidget_Status->scrollToBottom();
}

bool HostGameWindow::assignSpies()
{
    InsertToListWidget("HostGameWindow::assignSpies()" + QString::number(gameInfo.getNumberOfSpies()));

    //Assign all players Spy = 0
    for (int x = 0; x < 25; x++)
    {
        playerInfo[x].setSpyNumber(0);
    }

    if(gameInfo.getNumberOfSpies() == 0) return true;

    // ///////////////////////////////////////////////////////////////////////////
    //add some checking so that NumberOfSpies is !> number of players in any team.
    int numberOfPlayersInTeam1 = 0;
    int numberOfPlayersInTeam2 = 0;
    int numberOfPlayersInTeam3 = 0;

    //count the number of players in each team
    for (int x = 1; x < 9; x++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(x))       numberOfPlayersInTeam1++;
        if (gameInfo.getIsThisPlayerInTheGame(x+8))     numberOfPlayersInTeam2++;
        if (gameInfo.getIsThisPlayerInTheGame(x+16))    numberOfPlayersInTeam3++;
        if (gameInfo.getNumberOfTeams() < 3) numberOfPlayersInTeam3 = 8;  // Set to 8 so that we dont get a false positive in the checks below.
    }
    //exit if the number of spies is greater than number of players
    if(    (gameInfo.getNumberOfSpies() > numberOfPlayersInTeam1) ||
           (gameInfo.getNumberOfSpies() > numberOfPlayersInTeam2) ||
           (gameInfo.getNumberOfSpies() > numberOfPlayersInTeam3)  )
    {
        QMessageBox::critical(this,"Error","There are more spies than players in one or more of your teams.\n\nThis is illogical.\n\nPlease try again.");
        //ui->btn_StartGame->setEnabled(false);
        return false;
    }

    //issue warning if 50% or more of the players are spies
    if(     ( (gameInfo.getNumberOfSpies()*2) >= numberOfPlayersInTeam1) ||
            ( (gameInfo.getNumberOfSpies()*2) >= numberOfPlayersInTeam2) ||
            ( (gameInfo.getNumberOfSpies()*2) >= numberOfPlayersInTeam3)  )
            {
                int action = QMessageBox::question(this,tr("Are you sure?"), tr("More than 50% of your players in one team are spies.\nAre you sure this is what you want? \n\nPress OK to continue, or Cancel to set Spies to Zero."), QMessageBox::Ok | QMessageBox::Cancel);
                if (action == QMessageBox::Cancel) gameInfo.setNumberOfSpies(0);
            }

    // ////////////////////////////////////////////////////////
    //Pick a random player from each team as a Spy and assign a spy number (Spy = 1, Spy = 2)
    int spyPlayerNumber;
    bool loop = false;

    qDebug() << "";
    qDebug() << "------------------------------------------------";
    for (int spyNumber = 1; spyNumber <= gameInfo.getNumberOfSpies(); spyNumber++)
    {
        for (int teamNumber = 0; teamNumber < gameInfo.getNumberOfTeams(); teamNumber++)
        {
            loop = true;
            while (loop == true)
            {
                spyPlayerNumber = GetRandomNumber( (1+(teamNumber*8)), (8+(teamNumber*8)) );
                if (playerInfo[spyPlayerNumber].getSpyNumber() == 0 && gameInfo.getIsThisPlayerInTheGame(spyPlayerNumber))
                {
                    playerInfo[spyPlayerNumber].setSpyNumber(spyNumber);
                    qDebug() << "Player " << spyPlayerNumber <<  "\tis Spy #" << spyNumber  << "\tand is in the game - " << gameInfo.getIsThisPlayerInTheGame(spyPlayerNumber);
                    loop = false;
                }
            }
        }
        qDebug() << "------------------------------------------------";
    }
    qDebug() << "";

    return true;
}

int HostGameWindow::GetRandomNumber(int min, int max)
{
    return ((qrand() % ((max + 1) - min)) + min);
}

void HostGameWindow::changeMode(int mode)
{
    qDebug() << "HostGameWindow::changeMode" << mode;
    switch (mode)
    {
    case HOST_MODE:
        ui->btn_StartGame->setVisible(true);
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Cancel->setVisible(true);
        ui->btn_Cancel->setEnabled(true);
        ui->btn_SkipPlayer->setEnabled(true);
        ui->btn_SkipPlayer->setEnabled(true);
        ui->btn_Rehost->setVisible(false);
        ui->btn_Rehost->setEnabled(false);
        break;
    case COUNTDOWN_MODE:
        ui->btn_StartGame->setVisible(false);
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Cancel->setVisible(false);
        ui->btn_Cancel->setEnabled(true);
        ui->btn_SkipPlayer->setEnabled(false);
        ui->btn_SkipPlayer->setEnabled(true);
        ui->btn_Rehost->setVisible(false);
        ui->btn_Rehost->setEnabled(false);
        break;
    case GAME_MODE:
        ui->btn_StartGame->setVisible(false);
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Cancel->setVisible(false);
        ui->btn_Cancel->setEnabled(true);
        ui->btn_SkipPlayer->setEnabled(false);
        ui->btn_SkipPlayer->setEnabled(true);
        ui->btn_Rehost->setVisible(true);
        ui->btn_Rehost->setEnabled(true);
        break;
    case REHOST_MODE:

        break;
    case DEBRIEF_MODE:

        break;
    case SCOREBOARD_MODE:

        break;
    }
}

bool firstPass;
void HostGameWindow::on_btn_Rehost_clicked()
{
    if(ui->btn_Rehost->text() == "Cancel\nReHost")
    {
        timerReHost->stop();
        ui->btn_Rehost->setText("ReHost\nTagger");
        ui->btn_Rehost->setEnabled(true);
        rehostingActive = false;
    }
    else
    {
        reHostTagger = new ReHostTagger(this);
        ui->btn_Rehost->setEnabled(false);
        gameInfo.setPlayerToReHost(0);
        firstPass = true;
        reHostTagger->show();
        timerReHost->start(HOST_TIMER_MSEC);
    }
}

void HostGameWindow::TaggerReHost()
{
//    if(reHostTagger->getClosedWithoutSelectingPlayer() == true)                //If the Close button was pressed
//    {
//        ui->btn_Rehost->setEnabled(true);
//    }

    if(gameInfo.getPlayerToReHost() == 0) return;        //Means that the ReHost window is still open and no player is yet selected.

    if (firstPass)
    {
        changeMode(REHOST_MODE);
        currentPlayer = gameInfo.getPlayerToReHost();
        isThisPlayerHosted[currentPlayer] = false;
        rehostingActive = true;
        ui->label->setText("ReHosting " + playerInfo[currentPlayer].getPlayerName());
        lttoComms.setDontAnnounceGame(false);
        firstPass = false;
        ui->btn_Rehost->setText("Cancel\nReHost");
        ui->btn_Rehost->setEnabled(true);
    }
    sendingCommsActive = true;
    hostCurrentPlayer();                                //HostGameWindow::AddPlayer starts the CountDownTimer if(rehostingActive == true).
}

void HostGameWindow::BeaconSignal()
{
    if (lttoComms.getDontAnnounceGame()) return;
    //Check Game Type.

    //Set Beacon to match game type
    int beaconType = 2;         // Contested Zone, No Team

    //Send Beacon;
    lttoComms.sendPacket(BEACON, beaconType);
}


///////////////////------------------------------

void HostGameWindow::on_btn_FailSend_clicked()
{
    lttoComms.setDontAnnounceGame(true);
    expectingAckPlayerAssignment = true;
    sendAssignFailedMessage();
}

void HostGameWindow::on_btn_StartStopHosting_clicked()
{
    if (ui->btn_StartStopHosting->isChecked()) timerAnnounce->start(HOST_TIMER_MSEC);
    else timerAnnounce->stop();
}

void HostGameWindow::on_btn_StopStartBeacon_clicked()
{
    if (ui->btn_StopStartBeacon->isChecked()) timerBeacon->start(BEACON_TIMER_MSEC);
    else timerBeacon->stop();
}

///////////////////------------------------------

void HostGameWindow::deBriefTaggers()
{
    while (isThisPlayerHosted[currentPlayer] == false) currentPlayer++;                     // Find next player

    if (currentPlayer > 24)                                                                 // All players are debriefed
    {
        timerDeBrief->stop();
        currentPlayer = 0;
        ui->label->setText("The End");
        // Show Scores Window
        // Close this window
    }

    deBrief->RequestTagReports(currentPlayer);
}
