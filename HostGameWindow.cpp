#include "HostGameWindow.h"
#include "ui_HostGameWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include "Defines.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "StyleSheet.h"

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

    connect(timerAnnounce,          SIGNAL(timeout() ),                     this, SLOT(announceGame())            );
    connect(timerCountDown,         SIGNAL(timeout() ),                     this, SLOT(sendCountDown())           );
    connect(timerDeBrief,           SIGNAL(timeout() ),                     this, SLOT(deBriefTaggers())          );
    connect(timerAssignFailed,      SIGNAL(timeout() ),                     this, SLOT(sendAssignFailedMessage())        );
    connect(timerGameTimeRemaining, SIGNAL(timeout() ),                     this, SLOT(updateGameTimeRemaining()) );
    connect(timerReHost,            SIGNAL(timeout() ),                     this, SLOT(TaggerReHost()) );
    connect(timerBeacon,            SIGNAL(timeout() ),                     this, SLOT(BeaconSignal()) );
    connect(&lttoComms,             SIGNAL(RequestJoinGame(int,int,int, bool) ),  this, SLOT(AssignPlayer(int,int,int, bool)) );
    connect(&lttoComms,             SIGNAL(AckPlayerAssignment(int,int, bool) ),  this, SLOT(AddPlayerToGame(int,int, bool))  );
    connect(&serialUSBcomms,        SIGNAL(SerialPortFound(QString)),             this, SLOT(AddSerialPortToListWidget(QString)) );
    connect(&host,                  SIGNAL(AddToHostWindowListWidget(QString)),   this, SLOT(InsertToListWidget(QString)) );

    timerAnnounce->start(HOST_TIMER_MSEC);

    ui->btn_Cancel->setText("Cancel");
    ui->btn_Rehost->setEnabled(false);
    ui->listWidget_Status->setVisible(false);

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

    if(serialUSBcomms.getIsUSBinitialised() == false)
    {
        serialUSBcomms.initialiseUSBsignalsAndSlots();
    }

    if(tcpComms.getIsTCPinitialised() == false)
    {
        tcpComms.initialiseTCPsignalsAndSlots();
    }

    if(serialUSBcomms.getSerialCommsConnected() == false)
    {
        serialUSBcomms.setUpSerialPort();
    }

    //TODO: Get rid of these debug buttons
    ui->btn_AnnounceOnce->setVisible(false);
    ui->btn_DeBug->setVisible(false);
    ui->btn_DeBugSendTag->setVisible(false);
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
    closeHostGameWindow();
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

int     idleCount = 0;
void HostGameWindow::announceGame()
{
    sendingCommsActive = true;                                                              // Used to stop the class being destructed, if the Timer triggered.

    //Set Base Station LED status
    if (lttoComms.getTcpCommsConnected() == false)
    {
        ui->led_Status->setStyleSheet(myStyleSheet.getYellowButtonCss());
    }
    else
    {
        ui->led_Status->setStyleSheet(myStyleSheet.getGreenButtonCss());
    }


    if (currentPlayer != 0)                                                                 // Player 0 is the dummy player
    {
        while (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false)   currentPlayer++;  // Find next player

        if (lttoComms.getDontAnnounceGame() == false)
        {
            if (currentPlayer < 25)
            {
                ui->label->setText("Prepare to Host : " + playerInfo[currentPlayer].getPlayerName());
                lttoComms.sendLCDtext("Hosting"                                 , 1);
                lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName() , 2);
                lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
            }
        }
    }

    if (currentPlayer > 16 && gameInfo.getNumberOfTeams() == 2) currentPlayer = 25;         // Ignore players in Team 3

    if (currentPlayer >24)                                                                  // No more players to add
    {
        //TODO: Check if I need a 1.5 sec delay OR an additional AnnounceGame packet???
        // Wait 2.0 seconds, so that the last hosted players gets at least one more AnnounceGame Packet,
        // otherwise they will not respond to the countdown time (can fail to start game if countdown < 8 sec)
        //lttoComms.nonBlockingDelay(2000);

        currentPlayer = 0;                                                                  //Transmits global game data to keep guns sync'd
        // The last tagger that was hosted must see at least one more announce game packet to sync, otherwise they can miss a short countdown.
        hostCurrentPlayer();

        ui->label->setText("All players are hosted, press START ");
        lttoComms.sendLCDtext("All"            , 1);
        lttoComms.sendLCDtext("hosted"         , 2);
        lttoComms.sendLCDtext("STANDBY"        , 4);
        ui->btn_SkipPlayer->setVisible(false);
        ui->btn_StartGame->setEnabled(true);

    }

    hostCurrentPlayer();
    //  Note about Spies/Kings.
    //  We advertise the correct player settings here, then assign them to a different team/player in calculatePlayerTeam5bits().
}


void HostGameWindow::hostCurrentPlayer()
{
    if(lttoComms.getDontAnnounceGame() == true)                                             // We are receiving data, so dont announce.
    {
        //TODO: Test if this fixes the lockup when clicking Cancel when in this loop.
        if (closingWindow)
        {
            assignPlayerFailCount = 5;
            lttoComms.setDontAnnounceGame(false);
            return;
        }

        lttoComms.setMissedAnnounceCount(lttoComms.getMissedAnnounceCount()+1);             // Increment counter


        qDebug() << "HostGameWindow::hostCurrentPlayer() - DontAnnouceGame is active. Count = " << lttoComms.getMissedAnnounceCount();

        if (lttoComms.getMissedAnnounceCount() < 3) return;

        if (expectingAckPlayerAssignment == true)                                           // We appear to have missed the AckPlayerAssignment message.
        {
            qDebug() << "HostGameWindow::hostCurrentPlayer() - assignPlayerFailed !!"  ;
            assignPlayerFailed();                                                           // Call the AssignPlayerFailed routine.
        }
        else
        {
            lttoComms.setDontAnnounceGame(false);                                           // Start Announcing, as something went wrong
            qDebug() << "HostGameWindow::hostCurrentPlayer() - Reset/Clearing DontAnnounceGameFlag due to inactivity !!!!!!!";
        }
    }

    if (gameInfo.getIsLTARGame())   qDebug() << "HostGameWindow::hostCurrentPlayer() - LTAR mode = " << currentPlayer;
    else                            qDebug() << "HostGameWindow::hostCurrentPlayer() - Std mode = "  << currentPlayer;

    sound_Hosting->play();

    //Change TeamTags if required for Spies.
    bool wereCurrentPlayerTeamTagsActive = playerInfo[currentPlayer].getTeamTags();
    if(playerInfo[currentPlayer].getSpyNumber() != 0)   //Player is a spy
    {
        if(gameInfo.getIsSpiesTeamTagActive()) playerInfo[currentPlayer].setTeamTags(true);
    }

    //Let the user know what is happenning.
    if (lttoComms.getTcpCommsConnected() == true || serialUSBcomms.getSerialCommsConnected() == true)
    {
        if (gameInfo.getIsLTARGame())   InsertToListWidget("Announce LTAR Game : Player " + QString::number(currentPlayer));
        else                            InsertToListWidget("Announce Game : Player " + QString::number(currentPlayer));
    }
    else
    {
        InsertToListWidget("Trying to connect to Base Station");
        ui->label->setText("Connecting to Base Station......");
    }

    // Set all the variables.
    int gameTime;
    //  Set gameTime to gameLength or TimeRemaining
    if(rehostingActive)
    {
        gameTime = (remainingGameTime/60);
        if((remainingGameTime % 60) > 30) gameTime++;    // add a minute so that they dont finish early.
    }
    else                gameTime = gameInfo.getGameLength();

    //  Set the Reloads flag to match handicapped values
    if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads())  <  100)
         playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, true );
    else playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, false);

    //  Set the MegaTags flag to match handicapped values
    if      (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()) < 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_MEGAS_FLAG,   true);
    }
    else if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()) == 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_MEGAS_FLAG,   false);
    }

    int                                 GamePacket = gameInfo.getGameType();
    if (gameInfo.getIsReSpawnGame())    GamePacket = gameInfo.Custom;
    if (gameInfo.getIsLTARGame())       GamePacket = gameInfo.LtarGame;

    // Send the message.
    lttoComms.sendPacket(PACKET,   GamePacket                );
    lttoComms.sendPacket(DATA,     gameInfo.getGameID()      );
    lttoComms.sendPacket(DATA,     gameTime,              BCD);
    lttoComms.sendPacket(DATA,     playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getHealthTags()), BCD);

    if(gameInfo.getIsLTARGame() == false)
    {
        lttoComms.sendPacket(DATA, playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads()),   BCD);
    }
    else
    {
        lttoComms.sendPacket(DATA, playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads(gameInfo.getIsLTARGame()), 255),    BCD);
        lttoComms.sendPacket(DATA, playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads2(),255),   BCD);
    }
    lttoComms.sendPacket(DATA,     playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getShieldTime()), BCD);
    lttoComms.sendPacket(DATA,     playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()),   BCD);
    lttoComms.sendPacket(DATA,     playerInfo[currentPlayer].getPackedFlags1()   );
    lttoComms.sendPacket(DATA,     playerInfo[currentPlayer].getPackedFlags2()   );
    if(gameInfo.getIsLTARGame() )
        lttoComms.sendPacket(DATA, playerInfo[currentPlayer].getPackedFlags3()   );

    //  Add a name for Custom Game Types, otherwise skip.
    if (gameInfo.getGameType() == 0x0C || gameInfo.getIsReSpawnGame())
    {
        lttoComms.sendPacket(DATA, gameInfo.getNameChar1() );
        lttoComms.sendPacket(DATA, gameInfo.getNameChar2() );
        lttoComms.sendPacket(DATA, gameInfo.getNameChar3() );
        lttoComms.sendPacket(DATA, gameInfo.getNameChar4() );
    }
    if (gameInfo.getIsLTARGame() )
    {
        //TODO: Check that Starting Ammo works correctly - Br
        lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getStartingAmmo())   );     //Dont need BCD as LTARs talk Dec.
        lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getSleepTimeOut())   );     //Dont need BCD as LTARs talk Dec.
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
        playerDebugData =   "Debug...Player #" + QString::number(currentPlayer)
                            + ", Tags="     + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getHealthTags() ))
                            + ", Reloads="  + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads(gameInfo.getIsLTARGame()) ))
                            + ", Reloads2=" + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads2() ))
                            + ", Shields="  + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getShieldTime() ))
                            + ", Megas="    + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags() ))
                            + ", MedicMode:"+ QString::number(playerInfo[currentPlayer].getMedicMode() );
        InsertToListWidget(playerDebugData);

        playerDebugData =   "\tSlowTags:" + QString::number(playerInfo[currentPlayer].getSlowTags()  )
                            + ", TeamTags:" + QString::number(playerInfo[currentPlayer].getTeamTags()  )
                            + ", StartAmmo:"  + QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getStartingAmmo() ))
                            + ", SleepTime:"+ QString::number(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getSleepTimeOut() ))
                            + ", Spy#:"     + QString::number(playerInfo[currentPlayer].getSpyNumber() )
                            + ", King?:"    + QString::number(playerInfo[currentPlayer].getIsKing() );
        InsertToListWidget(playerDebugData);

        playerDebugData =   "\tFlag1 = "  + QString::number(playerInfo[currentPlayer].getPackedFlags1())
                            + ", Flag2 = "  + QString::number(playerInfo[currentPlayer].getPackedFlags2());
        if(gameInfo.getIsLTARGame()) playerDebugData += ", Flag3 = " + QString::number(playerInfo[currentPlayer].getPackedFlags2());
        InsertToListWidget(playerDebugData);
    }

    sendingCommsActive = false;
    if (closingWindow)
    {
        deleteLater();   // Delete the window, as the cancel button has been pressed.
    }
    return;
}


void HostGameWindow::AssignPlayer(int Game, int Tagger, int Flags, bool isLtar)

{
    if(currentPlayer > 24 || currentPlayer == 0)  //All taggers have been hosted, so ignore any new requests.
    {
        lttoComms.setDontAnnounceGame(false);
        return;
    }

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

        int         AssignPlayerPacket = ASSIGN_PLAYER;
        if (isLtar) AssignPlayerPacket = ASSIGN_LTAR_PLAYER;

        expectingAckPlayerAssignment = true;

        InsertToListWidget ("   AssignPlayer()" + QString::number(currentPlayer) + ", Tagger ID:" + QString::number(Tagger) );
        qDebug() << "\t\tHostGameWindow::AssignPlayer() " << currentPlayer << Game << Tagger << calculatePlayerTeam5bits(Flags) << isLtar << endl;
        if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.

        lttoComms.sendLCDtext("Adding"                                  , 1);
        lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName() , 2);
        lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);

        lttoComms.sendPacket(PACKET,  AssignPlayerPacket);
        lttoComms.sendPacket(DATA,    Game);
        lttoComms.sendPacket(DATA,    Tagger);
        lttoComms.sendPacket(DATA,    calculatePlayerTeam5bits(preferedTeam) );
        lttoComms.sendPacket(CHECKSUM);
    }
}

void HostGameWindow::AddPlayerToGame(int Game, int Tagger, bool isLtar)
{
    sound_PlayerAdded->play();

    lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName() , 1);
    lttoComms.sendLCDtext("Joined"                                  , 2);
    lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);

    if(isLtar)
    {
        qDebug() << "\t\tHostGameWindow::AddPlayerToGame() - LTAR MODE " << endl;
        lttoComms.sendPacket(PACKET, ASSIGN_LTAR_PLAYER_OK);
        lttoComms.sendPacket(DATA,   gameInfo.getGameID());
        lttoComms.sendPacket(DATA,    calculatePlayerTeam5bits(0) );
        lttoComms.sendPacket(CHECKSUM);
    }
    else
    {
        qDebug() << "\t\tHostGameWindow::AddPlayerToGame()" << currentPlayer << endl;
    }

    if(gameInfo.getGameID() != Game || playerInfo[currentPlayer].getTaggerID() != Tagger)
    {
        qDebug() << "\t\tHostGameWindow::AddPlayerToGame() - GameID or TaggerID mismatched !!!!!!   ERROR";
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

        lttoComms.sendLCDtext("Re-hosting"                                  , 1);
        lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName()     , 2);
        lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);

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
    lttoComms.setDontAnnounceFailedSignal(false);
    qDebug() << "\t\tHostGameWindow::AddPlayerToGame() - setDontAnnounce x2 = false";
    if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
}

void HostGameWindow::assignPlayerFailed()       //TODO: This is not working.
{
    lttoComms.setDontAnnounceGame(true);
    lttoComms.setDontAnnounceFailedSignal(false);
    sound_HostingMissedReply->play();
    qDebug() << "HostGameWindow::assignPlayerFailed() - starting Timer";
    timerAssignFailed->start(ASSIGNED_PLAYER_FAIL_TIMER);
    assignPlayerFailCount = 0;
    // The timerAssignedFailed is stopped when a message is sent to HostGameWindow::AddPlayerToGame()
}

void HostGameWindow::sendAssignFailedMessage()
{
    assignPlayerFailCount++;

    if(lttoComms.getDontAnnounceFailedSignal() == false)
    {  
        sound_HostingMissedReply->play();

        qDebug() << "HostGameWindow::sendAssignFailedMessage()  - Sending # " << assignPlayerFailCount;

        lttoComms.sendLCDtext("Attn"                                      , 1);
        lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName()   , 2);
        lttoComms.nonBlockingDelay(TEXT_SENT_DELAY*2);

        int                             AssignPlayerFailPacket = ASSIGN_PLAYER_FAIL;
        if (gameInfo.getIsLTARGame())   AssignPlayerFailPacket = ASSIGN_LTAR_PLAYER_FAIL;

        lttoComms.sendPacket(PACKET, AssignPlayerFailPacket                  );
        lttoComms.sendPacket(DATA,   gameInfo.getGameID()                    );
        lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].getTaggerID() );
        lttoComms.sendPacket(CHECKSUM                                        );

    }

    qDebug() << "HostGameWindow::sendAssignFailedMessage() - looping";

    if (assignPlayerFailCount >= 5)   //give up and start again
    {
        isThisPlayerHosted[currentPlayer] = false;
        playerInfo[currentPlayer].setTaggerID(0);
        timerAssignFailed->stop();
        lttoComms.setDontAnnounceGame(false);
        lttoComms.setDontAnnounceFailedSignal(false);
        expectingAckPlayerAssignment = false;
        assignPlayerFailCount = 0;
        qDebug() <<"HostGameWindow::sendAssignFailedMessage() - Counted to 5, I am now going away";
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

    //Check if this is a FreeForAll or Team Based game, as the player number system is different
    if (gameInfo.getNumberOfTeams() == 0)                     //Players 1 to 24, no teams
    {
        assignedTeamNumber = 0;
        assignedPlayerNumber = currentPlayer +7;              // For solo games: zero-based player number + 8
        playerTeam5bits = 8 + (currentPlayer-1);

        //set PlayerNumberInThisGame (for DeBrief),
        //but seeing as there are no Spies nor Kings, it is the same as currentPlayer.
        playerInfo[currentPlayer].setPlayerNumberInThisGame(currentPlayer);

        return playerTeam5bits;
    }
    else    //there are teams
    {
       //assign Teams (and swap if a spy)
       assignedTeamNumber = host.assignTeamsAndSwapIfSpy(currentPlayer);
    }
    playerTeam5bits = (assignedTeamNumber) << 3;

    //Check if this player is a spy and deal with it.
    if (playerInfo[currentPlayer].getSpyNumber() == 0)                      // Not a Spy
    {
        //assignedPlayerNumber = currentPlayer -  1;
        if      (assignedTeamNumber == 1) assignedPlayerNumber = currentPlayer -  1;  // 0 based player number
        else if (assignedTeamNumber == 2) assignedPlayerNumber = currentPlayer -  9;  // 0 based player number
        else if (assignedTeamNumber == 3) assignedPlayerNumber = currentPlayer - 17;  // 0 based player number
    }
    else
    {
        assignedPlayerNumber = host.swapSpyPlayers(currentPlayer);
    }

    //Check if this game is Kings and swap King with Player 1;
    if(gameInfo.getGameType() == Game::Kings2) assignedPlayerNumber = host.swapKingPlayers(currentPlayer);
    if(gameInfo.getGameType() == Game::Kings3) assignedPlayerNumber = host.swapKingPlayers(currentPlayer);

    playerTeam5bits += assignedPlayerNumber;

    //debug only
    int spyPlayerNumber = (assignedPlayerNumber + (8 * (assignedTeamNumber-1)) + 1);
    qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - Current Player = " << currentPlayer;;
    qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - Spy Player =     " << spyPlayerNumber << "\t- Team:" << assignedTeamNumber << ", Player:" << assignedPlayerNumber+1 << ", Spy# " << playerInfo[currentPlayer].getSpyNumber();
    qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - King Player ? =    " << playerInfo[currentPlayer].getIsKing() << " - Player #" << assignedPlayerNumber+1;
    qDebug() << "----";


    //calculate PlayerNumberInThisGame (for DeBrief)
    int playerNumInGame = ((assignedTeamNumber - 1) *8) + (assignedPlayerNumber + 1);
    playerInfo[currentPlayer].setPlayerNumberInThisGame(playerNumInGame);

    return playerTeam5bits;
}

void HostGameWindow::closeHostGameWindow()
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
    lttoComms.sendLCDtext(""          , 1);
    lttoComms.sendLCDtext("Wait for"  , 2);
    lttoComms.sendLCDtext("new Game"  , 3);
    lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
    if(lttoComms.getTcpCommsConnected())
    //if (sendingCommsActive == false) deleteLater();     //If this is true then the deleteLater is triggered at the end of hostCurrentPlayer(), to stop the app crashing.
    deleteLater();
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
        endGame();
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
             lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName()     , 1);
             lttoComms.sendLCDtext("is back"                                     , 2);
             lttoComms.sendLCDtext("& hosted !"                                  , 3);
             lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
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

        int                             CountDownPacket = COUNTDOWN;
        if (gameInfo.getIsLTARGame())   CountDownPacket = COUNTDOWN_LTAR;

        //TODO: Calculate and use 8bits for LTAR game players,
        //  and 4bits for LTTO games

        lttoComms.sendLCDtext("CountDown"                                , 1);
        lttoComms.sendLCDtext(QString::number(countDownTimeRemaining)    , 3);
        lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);

        lttoComms.sendPacket(PACKET ,   CountDownPacket);
        lttoComms.sendPacket(DATA,      gameInfo.getGameID() );
        lttoComms.sendPacket(DATA,      lttoComms.ConvertDecToBCD(countDownTimeRemaining));

        //getPlayersInTeam returns different data for LTAR and LTTTO games!
        lttoComms.sendPacket(DATA, gameInfo.getPlayersInTeam(1));
        lttoComms.sendPacket(DATA, gameInfo.getPlayersInTeam(2));
        lttoComms.sendPacket(DATA, gameInfo.getPlayersInTeam(3));
        //
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
    if (rehostingActive == false)
    {
        ui->label->setText("Game Time Remaining\n\n" + remainingMinutes + ":" + remainingSeconds);
        lttoComms.sendLCDtext("Game Time"                                , 1);
        lttoComms.sendLCDtext(remainingMinutes + ":" + remainingSeconds  , 3);
        lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
    }
    if (remainingGameTime == 0)
    {
        endGame();
    }
}

void HostGameWindow::on_btn_SkipPlayer_clicked()
{
    //TODO Why does this crash !!!!!
    if (currentPlayer > MAX_PLAYERS) return;

    if(deBrief) deBrief->setIsPlayerDeBriefed(true);
    gameInfo.setIsThisPlayerInTheGame(currentPlayer, false);
    lttoComms.setDontAnnounceGame(false);
    currentPlayer++;
}

void HostGameWindow::InsertToListWidget(QString lineText)
{
    ui->listWidget_Status->addItem(lineText);
    //if (ui->listWidget_Status->count() > 18) ui->listWidget_Status->takeItem(0);
    ui->listWidget_Status->scrollToBottom();
}


bool HostGameWindow::resetPlayersForNewGame()
{
    qDebug() << "\n\nHostGameWindow::resetPlayersForNewGame()";
    for(int players= 0; players < 25; players++)
    {
        isThisPlayerHosted[players] = false;
    }
    currentPlayer = 1;
    noMorePlayers = false;
    //firstPassThisPlayer = true;
    playerDebugNum = 0;

    //Pick Spies and the King
    host.pickTheSpies();
    if (gameInfo.getGameType() == Game::Kings2)  host.pickTheKing();
    if (gameInfo.getGameType() == Game::Kings3)  host.pickTheKing();

    //Reset a few game variables
    countDownTimeRemaining = DEFAULT_COUNTDOWN_TIME;
    remainingGameTime = gameInfo.getGameLength()*60;
    sendingCommsActive = false;
    rehostingActive = false;
    closingWindow = false;

    return true;
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

int beaconType = 0;
void HostGameWindow::BeaconSignal()
{
    if (lttoComms.getDontAnnounceGame()) return;
    if (rehostingActive) return;
    //Check Game Type.

    //TODO: Set Beacon to match game type
    if(gameInfo.getIsReSpawnGame()) beaconType = 3;
    else beaconType = 2;         // Contested Zone, No Team

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

///////////////////------------------------------


void HostGameWindow::endGame()
{
    deBrief = new DeBrief(this);
    connect(deBrief, SIGNAL(SendToHGWlistWidget(QString)), this, SLOT(InsertToListWidget(QString)) );
    timerGameTimeRemaining->stop();
    timerReHost->stop();
    timerBeacon->stop();
    timerDeBrief->start(DEBRIEF_TIMER_MSEC);
    currentPlayer = 1;                          // Set ready for DeBriefing to search for next player
    while (isThisPlayerHosted[currentPlayer] == false) currentPlayer++;
    ui->btn_StartGame->setEnabled(false);
    ui->btn_Rehost->setEnabled(false);
    ui->btn_Rehost->setVisible(false);
    ui->btn_Cancel->setEnabled(true);
    ui->btn_SkipPlayer->setVisible(true);
    ui->btn_SkipPlayer->setEnabled(true);
    ui->label->setText("DeBriefing " + playerInfo[currentPlayer].getPlayerName());
    lttoComms.sendLCDtext(""                 , 1);
    lttoComms.sendLCDtext("DeBriefing"       , 2);
    lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName(), 3);
    lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
    deBrief->prepareNewPlayerToDebrief(currentPlayer);
}

void HostGameWindow::deBriefTaggers()
{
    //Work out if player is debriefed
    deBrief->checkIfPlayerIsDebriefed();
    if(deBrief->getIsPlayerDeBriefed())
    {
        // the current player has been debriefed
        currentPlayer++;
        while (isThisPlayerHosted[currentPlayer] == false) currentPlayer++;
        if(currentPlayer < 25)
        {
            deBrief->prepareNewPlayerToDebrief(currentPlayer);
            ui->label->setText("Debriefing " + playerInfo[currentPlayer].getPlayerName());
            lttoComms.sendLCDtext(""                 , 1);
            lttoComms.sendLCDtext("DeBriefing"       , 2);
            lttoComms.sendLCDtext(playerInfo[currentPlayer].getPlayerName(), 3);
            lttoComms.nonBlockingDelay(TEXT_SENT_DELAY);
        }
    }

    // If currentPlayer is valid - send the message
    if (currentPlayer < 25)
    {
        deBrief->RequestTagReports();
    }
    else
    // All players are debriefed
    {
        timerDeBrief->stop();
        currentPlayer = 0;
        ui->label->setText("Finalising scores");

        //Sit and wait to allow time for slow messages to appear
        lttoComms.nonBlockingDelay(1500);
        ui->label->setText("Game Over");

        // Show Scores Window
        deBrief->calculateScores();
        deBrief->calculateRankings();
        deBrief->sendRankReport();

        if(!scoresWindow) scoresWindow = new ScoresWindow(this);
        scoresWindow->showFullScreen();

        //Send RankReport
        //deBrief->sendRankReport();



//        // Close this window
//        while (scoresWindow)
//        {
//            QEventLoop loop;
//            loop.exec();
//        }
//        qDebug() << "HostGameWindow::deBriefTaggers() - has left the loop";
//        closeHostGameWindow();
    }
}

void HostGameWindow::on_btn_ShowListWidget_clicked()
{
    ui->listWidget_Status->setVisible(true);
}

void HostGameWindow::on_btn_DeBugSendTag_clicked()
{
    lttoComms.sendPacket(TAG, 0);

    lttoComms.nonBlockingDelay(30);
    lttoComms.sendPacket(TAG, 48);
}

void HostGameWindow::on_btn_DeBug_clicked()
{
    tcpComms.DisconnectTCP();
}

void HostGameWindow::on_btn_StopHosting_clicked()
{
    if(timerAnnounce->isActive())
    {
        timerAnnounce->stop();
        ui->btn_StopHosting->setText("Start Hosting");
    }
    else
    {
        timerAnnounce->start();
        ui->btn_StopHosting->setText("Stop Hosting");
    }

}

void HostGameWindow::on_btn_AnnounceOnce_clicked()
{
    announceGame();
}

void HostGameWindow::on_btn_ForceAdd_clicked()
{
    if (currentPlayer > MAX_PLAYERS) return;
    currentPlayer++;
}
