#include "HostGameWindow.h"
#include "ui_HostGameWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include <QInputDialog>
#include "Defines.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"
#include "StyleSheet.h"
#include "FileLoadSave.h"

int playerDebugNum = 0;

HostGameWindow::HostGameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostGameWindow),
	timerAnnounce(nullptr),
	timerCountDown(nullptr),
	timerDeBrief(nullptr),
	timerAssignFailed(nullptr),
	timerGameTimeRemaining(nullptr),
	timerReHost(nullptr),
	timerBeacon(nullptr),
	timerAckNotReceived(nullptr),
	sound_Hosting(nullptr),
	sound_Countdown(nullptr),
	sound_HostingMissedReply(nullptr),
	sound_GoodLuck(nullptr),
	sound_PlayerAdded(nullptr)
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
	timerAckNotReceived		= new QTimer(this);

	lttoComms	= LttoComms::getInstance();
	lttoComms->initialise();

	host = new Hosting(this);

	connect(timerAnnounce,          SIGNAL(timeout() ),							  this, SLOT(announceGame())					);
	connect(timerCountDown,         SIGNAL(timeout() ),							  this, SLOT(sendCountDown())					);
	connect(timerDeBrief,           SIGNAL(timeout() ),							  this, SLOT(deBriefTaggers())					);
	connect(timerAssignFailed,      SIGNAL(timeout() ),							  this, SLOT(sendAssignFailedMessage())			);
	connect(timerGameTimeRemaining, SIGNAL(timeout() ),							  this, SLOT(updateGameTimeRemaining())			);
	connect(timerReHost,            SIGNAL(timeout() ),							  this, SLOT(TaggerReHost())					);
	connect(timerBeacon,            SIGNAL(timeout() ),							  this, SLOT(BeaconSignal())					);
	connect(timerAckNotReceived,    SIGNAL(timeout() ),							  this, SLOT(assignPlayerFailed())				);
    connect(lttoComms,              SIGNAL(RequestJoinGame(int,int,int,bool) ),   this, SLOT(AssignPlayer(int,int,int,bool))	);
    connect(lttoComms,              SIGNAL(AckPlayerAssignment(int,int,bool) ),   this, SLOT(AddPlayerToGame(int,int,bool))     );
	connect(host,                   SIGNAL(AddToHostWindowListWidget(QString) ),  this, SLOT(InsertToListWidget(QString))		);
	connect(lttoComms,				SIGNAL(BattVoltsReceived(QString) ),		  this, SLOT(UpdateBatteryDisplay(QString))		);
	connect(lttoComms,				SIGNAL(FirmwareVersionReceived(QString) ),	  this, SLOT(checkFirmwareVersion(QString))		);

    timerAnnounce->start(HOST_TIMER_MSEC);
	lttoComms->setHostingCommsActive(true);

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

	gameInfo.setGameID(host->getRandomNumber(1,255));
	firmWareVersionReceived = false;
	beaconType = 0;

	ui->listWidget_Status->setVisible(false);

	removePhantomTeam3players();
	sendGameSettingsToLog();
	disableHostSleep(true);

    nextPlayer = 0;
}

HostGameWindow::~HostGameWindow()
{
    timerAnnounce->stop();
    timerCountDown->stop();
    timerDeBrief->stop();
    timerAssignFailed->stop();
    timerGameTimeRemaining->stop();
    timerReHost->stop();
	timerBeacon->stop();
	lttoComms->setHostingCommsActive(false);
	disableHostSleep(false);
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
	timerBeacon->stop();
	disableHostSleep(false);
	lttoComms->setHostingCommsActive(false);
}

void HostGameWindow::on_btn_Cancel_clicked()
{
	lttoComms->sendDisconnectClient();
	closeHostGameWindow();
}

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////

void HostGameWindow::announceGame()
{
	sendingCommsActive = true;                                                              // Used to stop the class being destructed, if the Timer triggered.

    //Set Base Station LED status
	if (lttoComms->getTcpCommsConnected() == false)
    {
        ui->led_Status->setStyleSheet(myStyleSheet.getYellowButtonCss());
    }
    else
    {
        ui->led_Status->setStyleSheet(myStyleSheet.getGreenButtonCss());
    }

	if (lttoComms->checkIPaddress() != true && lttoComms->getSerialUSBcommsConnected() != true)
	{
		ui->led_Status->setStyleSheet(myStyleSheet.getRedButtonCss());
		//qDebug() << "HostGameWindow::announceGame() - checkIPaddress failed.";
		setPromptText("Connect WiFi to Combobulator");
		setNextPlayerText("Password = Lasertag42");
		ui->led_Status->setStyleSheet(myStyleSheet.getRedButtonCss());
		return;
	}

	hostCurrentPlayer();
	//  Note about Spies/Kings.
	//  We advertise the correct player settings here, then assign them to a different team/player in calculatePlayerTeam5bits().

    if (currentPlayer != 0)                                                                 // Player 0 is the dummy player
    {
		while (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false) currentPlayer++;  // Find next player

		while(nextPlayer <= currentPlayer)
		{
			nextPlayer++;
			while (gameInfo.getIsThisPlayerInTheGame(nextPlayer) == false) nextPlayer++;
		}

		if (lttoComms->getDontAnnounceGame() == false)
		{
			if (currentPlayer < 25)
			{
                QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);
                QString nextTeamNumText = "Team " + QString::number(((nextPlayer-1)/8)+1);

                if(gameInfo.getNumberOfTeams() ==  0) teamNumText = "";

                setPromptText("Prepare to Host :<br>" + teamNumText + " " + playerInfoTemp[currentPlayer].getPlayerName() );
                lttoComms->sendLCDtext("Hosting"                                    , 1, false);
                lttoComms->sendLCDtext(teamNumText                                  , 2, false);
                lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName(), 3, false);

				lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
				if(nextPlayer < 25)
				{
                    setNextPlayerText       ("Next Up : " + nextTeamNumText + " " + playerInfoTemp[nextPlayer].getPlayerName() );
                    lttoComms->sendLCDtext(0, 55, "Next:" + nextTeamNumText + " " + playerInfoTemp[nextPlayer].getPlayerName(), 1, 1, false, true);
				}
				else
				{
                    setNextPlayerText             ("");
					lttoComms->sendLCDtext(5, 50, " ", 1, 1, false, true);  //trigger the drawScreen command
				}
             }
         }
    }

	if (currentPlayer > 16 && gameInfo.getNumberOfTeams() == 2)          // Ignore players in Team 3
	{
		currentPlayer = MAX_PLAYERS + 1;
		nextPlayer	  = MAX_PLAYERS + 1;
	}

	if (currentPlayer > MAX_PLAYERS)                                                                  // No more players to add
    {
        //TODO: Check if I need a 1.5 sec delay OR an additional AnnounceGame packet???
        // Wait 2.0 seconds, so that the last hosted players gets at least one more AnnounceGame Packet,
        // otherwise they will not respond to the countdown time (can fail to start game if countdown < 8 sec)
		lttoComms->nonBlockingDelay(1000);

		currentPlayer = 0;                                 //Transmits global game data to keep guns sync'd
        // The last tagger that was hosted must see at least one more announce game packet to sync, otherwise they can miss a short countdown.
        hostCurrentPlayer();
		qDebug() << "HostGameWindow::announceGame() - All players Hosted";
		ui->label_Prompt->setText("<html><head/><body><p><span style= font-size:64pt; font-weight:600;>All players are hosted, press START ");
		lttoComms->sendLCDtext("All"            , 1, false);
		lttoComms->sendLCDtext("hosted"         , 2, false);
		lttoComms->sendLCDtext("STANDBY"        , 4,  true);
        ui->btn_SkipPlayer->setVisible(false);
        ui->btn_StartGame->setEnabled(true);
		setNextPlayerText("");

    }
}


void HostGameWindow::hostCurrentPlayer()
{
	if(lttoComms->getDontAnnounceGame() == true)                                             // We are receiving data, so dont announce.
    {
        //TODO: Test if this fixes the lockup when clicking Cancel when in this loop.
        if (closingWindow)
        {
			qDebug() << endl << "HostGameWindow::hostCurrentPlayer() - closingWindow == TRUE";
			qDebug() << "but we are still locking up........" << endl;
			assignPlayerFailCount = 5;
			lttoComms->setDontAnnounceGame(false);
            return;
        }

		lttoComms->setMissedAnnounceCount(lttoComms->getMissedAnnounceCount()+1);             // Increment counter


		qDebug() << "HostGameWindow::hostCurrentPlayer() - DontAnnouceGame is active. Count = " << lttoComms->getMissedAnnounceCount();
		lttoComms->sendHeartBeat();

		if (lttoComms->getMissedAnnounceCount() < 5) return;

//        if (expectingAckPlayerAssignment == true)                                           // We appear to have missed the AckPlayerAssignment message.
//        {
//            qDebug() << "HostGameWindow::hostCurrentPlayer() - assignPlayerFailed !!"  ;
//            assignPlayerFailed();                                                           // Call the AssignPlayerFailed routine.
//        }
//        else
        {
			lttoComms->setDontAnnounceGame(false);                                           // Start Announcing, as something went wrong
            qDebug() << "HostGameWindow::hostCurrentPlayer() - Reset/Clearing DontAnnounceGameFlag due to inactivity !!!!!!!";
        }
    }

//    if (gameInfo.getIsLTARGame())   qDebug() << "HostGameWindow::hostCurrentPlayer() - LTAR mode = " << currentPlayer;
//    else                            qDebug() << "HostGameWindow::hostCurrentPlayer() - Std mode = "  << currentPlayer;

    sound_Hosting->play();

    //Change TeamTags if required for Spies.
    bool wereCurrentPlayerTeamTagsActive = playerInfo[currentPlayer].getTeamTags();
    if(playerInfo[currentPlayer].getSpyNumber() != 0)   //Player is a spy
    {
        if(gameInfo.getIsSpiesTeamTagActive()) playerInfo[currentPlayer].setTeamTags(true);
    }

    //Let the user know what is happenning.
	if(lttoComms->getConnectionStatus())
    {
        if (gameInfo.getIsLTARGame())   InsertToListWidget("Announce LTAR Game : Player " + QString::number(currentPlayer));
        else                            InsertToListWidget("Announce Game : Player " + QString::number(currentPlayer));
	}
    else
    {
		InsertToListWidget("Trying to connect to Base Station");
		setPromptText("...Connecting to Base Station......");
		ui->label_NextPlayer->setText("");
		//<html><head/><body><p><span style=" font-size:64pt; font-weight:600;">Initialising Data. Standby.......</span></p></body></html>
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
	if(playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads())  <  100)
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
	lttoComms->sendPacket(PACKET,   GamePacket                );
	lttoComms->sendPacket(DATA,     gameInfo.getGameID()      );
qDebug() << "HostGameWindow::hostCurrentPlayer(): gameID =" << gameInfo.getGameID();
	lttoComms->sendPacket(DATA,     gameTime,              BCD);
	lttoComms->sendPacket(DATA,     playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getHealthTags()), BCD);

    if(gameInfo.getIsLTARGame() == false)
    {
		lttoComms->sendPacket(DATA, playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads()),   BCD);
    }
    else
    {
		lttoComms->sendPacket(DATA, playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads(gameInfo.getIsLTARGame()), 255),    BCD);
		lttoComms->sendPacket(DATA, playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads2(),255),   BCD);
    }
	lttoComms->sendPacket(DATA,     playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getShieldTime()), BCD);
	lttoComms->sendPacket(DATA,     playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()),   BCD);
	lttoComms->sendPacket(DATA,     playerInfo[currentPlayer].getPackedFlags1()   );
	lttoComms->sendPacket(DATA,     playerInfo[currentPlayer].getPackedFlags2()   );
    if(gameInfo.getIsLTARGame() )
		lttoComms->sendPacket(DATA, playerInfo[currentPlayer].getPackedFlags3()   );

    //  Add a name for Custom Game Types, otherwise skip.
    if (gameInfo.getGameType() == 0x0C || gameInfo.getIsReSpawnGame())
    {
		lttoComms->sendPacket(DATA, gameInfo.getNameChar1() );
		lttoComms->sendPacket(DATA, gameInfo.getNameChar2() );
		lttoComms->sendPacket(DATA, gameInfo.getNameChar3() );
		lttoComms->sendPacket(DATA, gameInfo.getNameChar4() );
    }
    if (gameInfo.getIsLTARGame() )
    {
//Starting Ammo is broken !!!!
//TODO: Check that Starting Ammo works correctly - Bryan says NO!
        qDebug() << "LATR Starting Ammo =" << playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getStartingAmmo());

		lttoComms->sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getStartingAmmo())   );     //Dont need BCD as LTARs talk Dec.
        lttoComms->sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getSleepTimeOut())   );     //Dont need BCD as LTARs talk Dec.
    }
    //send the Checksum
	lttoComms->sendPacket(CHECKSUM);

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
	if(currentPlayer > MAX_PLAYERS || currentPlayer == 0)  //All taggers have been hosted, so ignore any new requests.
    {
		lttoComms->setDontAnnounceGame(false);
        return;
    }

	lttoComms->setDontAnnounceGame(true);

    //Check if the gameID from the tagger is correct.
    if(gameInfo.getGameID() != Game)
    {
        qDebug() << "HostGameWindow::AssignPlayer - GameID mismatch !!!!!!      ERROR";
		lttoComms->setDontAnnounceGame(false);
        return;
    }
	//Set currentTagger so that LttoComms::processPacket() can ignore other taggers that respond.
	lttoComms->setCurrentTaggerBeingHosted(Tagger);

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

        QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);

        lttoComms->sendLCDtext("Adding"                                      , 1, false);
        lttoComms->sendLCDtext(teamNumText                                   , 2, false);
        lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName() , 3,  true);
		lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);

		lttoComms->sendPacket(PACKET,  AssignPlayerPacket);
		lttoComms->sendPacket(DATA,    Game);
		lttoComms->sendPacket(DATA,    Tagger);
		lttoComms->sendPacket(DATA,    calculatePlayerTeam5bits(preferedTeam) );
		lttoComms->sendPacket(CHECKSUM);
		timerAckNotReceived->start(ACK_NOT_RECEIVED_MSEC);
		qDebug() << "t\t\tHostGameWindow::AssignPlayer() starting timerAckNotReceived";
    }
}

void HostGameWindow::AddPlayerToGame(int Game, int Tagger, bool isLtar)
{
	if(gameInfo.getGameID() != Game || playerInfo[currentPlayer].getTaggerID() != Tagger)
	{
		qDebug() << "\t\tHostGameWindow::AddPlayerToGame() - GameID or TaggerID mismatched !!!!!!   ERROR" << Game << gameInfo.getGameID() << ":" << Tagger << playerInfo[currentPlayer].getTaggerID();
		lttoComms->setDontAnnounceGame(false);
		return;
	}
    QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);
    lttoComms->sendLCDtext(teamNumText                                  , 1, false);
    lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName(), 2, false);
    lttoComms->sendLCDtext("Joined"                                     , 3,  true);
	lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);

    if(isLtar)
    {
		qDebug() << "\t\tHostGameWindow::AddPlayerToGame() - LTAR MODE " << endl;
		InsertToListWidget ("HostGameWindow::AddPlayerToGame() - LTAR MODE");
		//Send LTAR_RELEASE message.
		lttoComms->sendPacket(PACKET, ASSIGN_LTAR_PLAYER_OK);
		lttoComms->sendPacket(DATA,   gameInfo.getGameID());
		lttoComms->sendPacket(DATA,   calculatePlayerTeam5bits(0) );
		lttoComms->sendPacket(CHECKSUM);
		expectingLtarAssignSuccess = true;
    }
    else
    {
		qDebug() << "\t\tHostGameWindow::AddPlayerToGame()" << currentPlayer << endl;
    }

	timerAckNotReceived->stop();
	timerAssignFailed->stop();
	sound_PlayerAdded->play();
    InsertToListWidget("   AddPlayerToGame()" + QString::number(currentPlayer));
	lttoComms->setCurrentTaggerBeingHosted(0);
	lttoComms->setDontAnnounceGame(false);
	isThisPlayerHosted[currentPlayer] = true;
    expectingAckPlayerAssignment = false;


    if(rehostingActive)
    {
        timerReHost->stop();
        //gameInfo.setPlayerToReHost(0);
		setPromptText(playerInfo[currentPlayer].getPlayerName() + " has joined.");

		lttoComms->sendLCDtext("Re-hosting"                                  , 1, false);
        lttoComms->sendLCDtext(teamNumText                                   , 2, false);
        lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName() , 3,  true);
		lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);

        countDownTimeRemaining = (DEFAULT_COUNTDOWN_TIME + 5);
        ui->btn_StartGame->setEnabled(true);
        //ui->btn_StartGame->setText("End\nGame");
        ui->btn_Rehost->setText("Rehost Tagger");
		lttoComms->nonBlockingDelay(1000);
        timerCountDown->start(1000);    // Auto start the countdown to get the player into the game ASAP.
    }
    else
    {
	if (currentPlayer != 0) currentPlayer++;
    }

	lttoComms->setDontAnnounceGame(false);
	lttoComms->setDontAnnounceFailedSignal(false);
    if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
}

void HostGameWindow::assignPlayerFailed()       //This must be triggered within 2000mS of the AssignPlayer message being sent,
												// or the taggers will not respond (they timeout and stop listening).
{
	timerAckNotReceived->stop();
	lttoComms->setDontAnnounceGame(true);
	lttoComms->setDontAnnounceFailedSignal(false);
	qDebug() << "*** HostGameWindow::assignPlayerFailed() - starting Timer ***";
    timerAssignFailed->start(ASSIGNED_PLAYER_FAIL_TIMER);
    assignPlayerFailCount = 0;
	lttoComms->setCurrentTaggerBeingHosted(0);
    // The timerAssignedFailed is stopped when a message is sent to HostGameWindow::AddPlayerToGame()
}

void HostGameWindow::sendAssignFailedMessage()
{
	assignPlayerFailCount++;

	if(lttoComms->getDontAnnounceFailedSignal() == false)
    {  
		lttoComms->setDontAnnounceGame(true);
		sound_HostingMissedReply->play();

		qDebug() << "HostGameWindow::sendAssignFailedMessage()  - Sending # " << assignPlayerFailCount;
		InsertToListWidget ("HostGameWindow::sendAssignFailedMessage()  - Sending # " + QString::number(assignPlayerFailCount));

		lttoComms->sendLCDtext("Attn"                                      , 1, false);
		lttoComms->sendLCDtext(playerInfo[currentPlayer].getPlayerName()   , 2,  true);
		lttoComms->nonBlockingDelay(TEXT_SENT_DELAY*2);

        int                             AssignPlayerFailPacket = ASSIGN_PLAYER_FAIL;
        if (gameInfo.getIsLTARGame())   AssignPlayerFailPacket = ASSIGN_LTAR_PLAYER_FAIL;

		lttoComms->sendPacket(PACKET, AssignPlayerFailPacket                  );
		lttoComms->sendPacket(DATA,   gameInfo.getGameID()                    );
		lttoComms->sendPacket(DATA,   playerInfo[currentPlayer].getTaggerID() );
		lttoComms->sendPacket(CHECKSUM                                        );
    }

	//qDebug() << "HostGameWindow::sendAssignFailedMessage() - looping";
	//InsertToListWidget ("HostGameWindow::sendAssignFailedMessage() - looping");

    if (assignPlayerFailCount >= 5)   //give up and start again
    {
        isThisPlayerHosted[currentPlayer] = false;
        playerInfo[currentPlayer].setTaggerID(0);
        timerAssignFailed->stop();
		lttoComms->setDontAnnounceGame(false);
		lttoComms->setDontAnnounceFailedSignal(false);
        expectingAckPlayerAssignment = false;
        assignPlayerFailCount = 0;
		//qDebug() <<"HostGameWindow::sendAssignFailedMessage() - Counted to 5, I am going away now";

		//Manually add the player to the game (a kludge, but do it for now)
		//TODO: Hmmmm. Fix this kludge
		//Seems to completely break LTAR hosting, so bypass to see if it helps
//		if(gameInfo.getIsLTARGame() != true)
//		{
//			AddPlayerToGame(gameInfo.getGameID(), lttoComms->getCurrentTaggerBeingHosted(), gameInfo.getIsLTARGame() );
//			qDebug() << "----------------------------------------------";
//			qDebug() << "DANGER WILL ROBINSON - PLAYER FORCED INTO GAME";
//			qDebug() << "----------------------------------------------";
//			InsertToListWidget ("No Tagger Ack recvd - PLAYER FORCED INTO GAME!" );
//		}


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
		//qDebug() << "HostGameWindow::calculatePlayerTeam5bit() - Requested Team = " << requestedTeam;
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
	   assignedTeamNumber = host->assignTeamsAndSwapIfSpy(currentPlayer);
    }
	//BUG: Is this right or wrong????
	//if(gameInfo.getIsLTARGame())	playerTeam5bits = (assignedTeamNumber) << 4;
	//else							playerTeam5bits = (assignedTeamNumber) << 3;
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
		assignedPlayerNumber = host->swapSpyPlayers(currentPlayer);
    }

    //Check if this game is Kings and swap King with Player 1;
	if(gameInfo.getGameType() == Game::Kings2) assignedPlayerNumber = host->swapKingPlayers(currentPlayer);
	if(gameInfo.getGameType() == Game::Kings3) assignedPlayerNumber = host->swapKingPlayers(currentPlayer);

    playerTeam5bits += assignedPlayerNumber;

    //debug only
	//int spyPlayerNumber = (assignedPlayerNumber + (8 * (assignedTeamNumber-1)) + 1);
	//qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - Current Player = " << currentPlayer;;
	//qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - Spy Player =     " << spyPlayerNumber << "\t- Team:" << assignedTeamNumber << ", Player:" << assignedPlayerNumber+1 << ", Spy# " << playerInfo[currentPlayer].getSpyNumber();
	//qDebug() << "   HostGameWindow::calculatePlayerTeam5bits() - King Player ? =    " << playerInfo[currentPlayer].getIsKing() << " - Player #" << assignedPlayerNumber+1;
	//qDebug() << "----";


    //calculate PlayerNumberInThisGame (for DeBrief)
    int playerNumInGame = ((assignedTeamNumber - 1) *8) + (assignedPlayerNumber + 1);
    playerInfo[currentPlayer].setPlayerNumberInThisGame(playerNumInGame);

    return playerTeam5bits;
}

void HostGameWindow::closeHostGameWindow()
{
	qDebug() << "HostGameWindow::closeHostGameWindow() - says Good-bye";
	timerAnnounce->stop();
	timerCountDown->stop();
	timerDeBrief->stop();
	timerAssignFailed->stop();
	timerGameTimeRemaining->stop();
	timerReHost->stop();
	timerBeacon->stop();

	lttoComms->sendLCDtext(""          , 1, false);
	lttoComms->sendLCDtext("Wait for"  , 2, false);
	lttoComms->sendLCDtext("new Game"  , 3,  true);
	lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
	lttoComms->nonBlockingDelay(500);
//	qDebug() << " _____ HostGameWindow::closeHostGameWindow() - Restarting ESP";
//	lttoComms->sendPing("ESP-Please Restart");
//	lttoComms->sendEspRestart();
//	lttoComms->sendPing("ESP-Please Restart"); //required to get the reset to work.
//	lttoComms->nonBlockingDelay(500);
	lttoComms->closePorts();
	disableHostSleep(false);

	closingWindow = true;
    ui->btn_Cancel->setEnabled(false);
	setPromptText("Cancelling......");

	//if (sendingCommsActive == false) deleteLater();     //If this is true then the deleteLater is triggered at the end of hostCurrentPlayer(), to stop the app crashing.
	emit closingHostGameWindow();
	deleteLater();
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
		if(gameInfo.getNumberOfPlayersInGame() == 0)
		{
			QMessageBox::critical(nullptr,"Error","There are no players in the game.\n\nThis is illogical.\n\nContinuing for Demo purposes only");
		}
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
		lttoComms->sendLCDtext(""				, 1, false);
		lttoComms->sendLCDtext("Good"           , 2, false);
		lttoComms->sendLCDtext("Luck"           , 3,  true);
		sound_Countdown->stop();
        sound_GoodLuck->play();
        timerCountDown->stop();
		sendingCommsActive = false;					// Otherwise it is


        if (rehostingActive == false)               //Ignore the next section if the game is running.
        {
            timerGameTimeRemaining->start(1000);
			remainingGameTime = (lttoComms->ConvertBCDtoDec(gameInfo.getGameLength()) * 60) + 2;  // Add a couple of seconds to match the taggers, who start the clock AFTER the Good Luck message.
			InsertToListWidget("Game commencing !!!");
			setPromptText("Game Commencing !!!");
			qDebug() << "hostGameWindow::sendCountDown() - Game Commencing";
			timerBeacon->start(BEACON_TIMER_MSEC);
        }
        else
        {
			 setPromptText("Player" + QString::number(currentPlayer) + " is back in the game");
			 lttoComms->sendLCDtext(playerInfo[currentPlayer].getPlayerName()     , 1, false);
			 lttoComms->sendLCDtext("is back"                                     , 2, false);
			 lttoComms->sendLCDtext("& hosted !"                                  , 3,  true);
			 lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
             rehostingActive = false;
        }

		lttoComms->setDontAnnounceGame(false);
        ui->btn_Cancel->setText("Close");
        ui->btn_Rehost->setVisible(true);
        ui->btn_Rehost->setEnabled(true);
        ui->btn_StartGame->setText("End\nGame");
        ui->btn_StartGame->setEnabled(true);
        ui->btn_Cancel->setEnabled(false);
		qDebug() << "\n*** The Game has started ***";
		qDebug() << "\t*** The Game has started ***";
		qDebug() << "\t\t*** The Game has started ***";
		qDebug() << "\t\t\t*** The Game has started ***";
		qDebug() << "\t\t\t\t*** The Game has started ***";
		qDebug() << "\t\t\t\t\t*** The Game has started ***\n";

        //TODO:Change the form to show the DeBrief stuff - use a State Machine?
    }
    else                                // Send the Countdown Signal
    {
        sound_Countdown->play();

        int                             CountDownPacket = COUNTDOWN;
        if (gameInfo.getIsLTARGame())   CountDownPacket = COUNTDOWN_LTAR;

        //TODO: Calculate and use 8bits for LTAR game players,
        //  and 4bits for LTTO games

		lttoComms->sendLCDtext("CountDown"                                , 1, false);
		lttoComms->sendLCDtext(QString::number(countDownTimeRemaining)    , 3,  true);
		lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);

		lttoComms->sendPacket(PACKET ,   CountDownPacket);
		lttoComms->sendPacket(DATA,      gameInfo.getGameID() );
		lttoComms->sendPacket(DATA,      lttoComms->ConvertDecToBCD(countDownTimeRemaining));

        //getPlayersInTeam returns different data for LTAR and LTTTO games!
		lttoComms->sendPacket(DATA, gameInfo.getPlayersInTeam(1));
		lttoComms->sendPacket(DATA, gameInfo.getPlayersInTeam(2));
		lttoComms->sendPacket(DATA, gameInfo.getPlayersInTeam(3));
        //
		lttoComms->sendPacket(CHECKSUM);

        InsertToListWidget("CountDownTime = " + QString::number(countDownTimeRemaining));
		setPromptText("CountDownTime\n\n" + QString::number(countDownTimeRemaining));
		//qDebug() << "CountDownTime = " + QString::number(countDownTimeRemaining);
        countDownTimeRemaining--;
    }
}

void HostGameWindow::updateGameTimeRemaining()
{
    if (!reHostTagger) ui->btn_Rehost->setEnabled(true);
    remainingGameTime--;
	if (remainingGameTime > (lttoComms->ConvertBCDtoDec(gameInfo.getGameLength()) * 60)) return;   // Ignore the extra 2 seconds we added to the clock.
    QString remainingMinutes = QString::number(remainingGameTime / 60);
    QString remainingSeconds = QString::number(remainingGameTime % 60);
    if (remainingMinutes.length() == 1) remainingMinutes.prepend("0");
    if (remainingSeconds.length() == 1) remainingSeconds.prepend("0");

	if (rehostingActive == false)
    {
		setPromptText("Game Time Remaining\n\n" + remainingMinutes + ":" + remainingSeconds);

		if(gameInfo.getPowerSaveMode() == false)
		//if PowerSaving is true, then don't send gametime, to reduce WiFi traffic.
		{
			lttoComms->sendLCDtext("Game Time"                                , 1, false);
			lttoComms->sendLCDtext(remainingMinutes + ":" + remainingSeconds  , 3,  true);
			lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
		}
		else qDebug() << "HostGameWindow::BeaconSignal() - Disabled !	PowerSaveMode is active.";
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
	//TODO1: Redraw PlayerScreen, otherwise highlights are wrong.
	lttoComms->setDontAnnounceGame(false);
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
	host->pickTheSpies();
	if (gameInfo.getGameType() == Game::Kings2)  host->pickTheKing();
	if (gameInfo.getGameType() == Game::Kings3)  host->pickTheKing();

    //Reset a few game variables
	resetScores();
    countDownTimeRemaining = DEFAULT_COUNTDOWN_TIME;
    remainingGameTime = gameInfo.getGameLength()*60;
    sendingCommsActive = false;
    rehostingActive = false;
    closingWindow = false;

	gameInfo.setGameID(host->getRandomNumber(1,255));

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
        QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);
        setPromptText("ReHosting " + playerInfoTemp[currentPlayer].getPlayerName());
        lttoComms->sendLCDtext("ReHosting"                                  , 1, false);
        lttoComms->sendLCDtext(teamNumText                                  , 2, false);
        lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName(), 3,  true);
		lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
		lttoComms->setDontAnnounceGame(false);
        firstPass = false;
        ui->btn_Rehost->setText("Cancel\nReHost");
        ui->btn_Rehost->setEnabled(true);
    }

	sendingCommsActive = true;
    hostCurrentPlayer();                                //HostGameWindow::AddPlayer starts the CountDownTimer if(rehostingActive == true).
}


void HostGameWindow::BeaconSignal()
{
	qDebug() << "\tHostGameWindow::BeaconSignal() triggered";
	if (lttoComms->getDontAnnounceGame()) return;
    if (rehostingActive) return;
    //Check Game Type.
    //TODO: Set Beacon to match game type
	if(gameInfo.getIsReSpawnGame()) beaconType = 3;
    else beaconType = 2;         // Contested Zone, No Team

	//Beta
	if(playerInfo->getBitFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG)) beaconType = 3;
	qDebug() << "\t___HostGameWindow::BeaconSignal() - playerInfo->getBitFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG) =" << playerInfo->getBitFlags2(SUPPLY_ZONES_REFILL_TAGS_FLAG);

    if(playerInfo->getBitFlags2(HOSTILE_ZONES_FLAG)) beaconType = 99;
    qDebug() << "\t___HostGameWindow::BeaconSignal() - playerInfo->getBitFlags2(HOSTILE_ZONES_FLAG) =" << playerInfo->getBitFlags2(HOSTILE_ZONES_FLAG);

    if(gameInfo.getIsLTARGame())
	{
		if(playerInfo->getBitFlags3(SUPPLY_ZONES_REFILL_AMMO))		beaconType = 3;
		if(playerInfo->getBitFlags3(SUPPLY_ZONES_REFILL_SHIELDS))	beaconType = 3;
		qDebug() << "\t___HostGameWindow::BeaconSignal() - playerInfo->getBitFlags3(SUPPLY_ZONES_REFILL_AMMO) =" << playerInfo->getBitFlags3(SUPPLY_ZONES_REFILL_AMMO);
		qDebug() << "\t___HostGameWindow::BeaconSignal() - playerInfo->getBitFlags3(SUPPLY_ZONES_REFILL_SHIELDS) =" << playerInfo->getBitFlags3(SUPPLY_ZONES_REFILL_SHIELDS);
	}

    //Send Beacon;
	if(gameInfo.getPowerSaveMode() == false)
	//if PowerSaving is true, then don't send beacon, to reduce WiFi traffic.
	{
 //UPTO HERE       if (beaconType == 99)   lttoComms->sendPacket(TAG, )
 //           else                lttoComms->sendPacket(BEACON, beaconType);    
        lttoComms->sendPacket(BEACON, beaconType);

		qDebug() << "\t\tHostGameWindow::BeaconSignal() - Type =" << beaconType;
	}
	else qDebug() << "HostGameWindow::BeaconSignal() - Disabled !	PowerSaveMode is active.";
}

///////////////////------------------------------


void HostGameWindow::endGame()
{
	qDebug() << "\n\t\t\t\t\t*** The Game has ended ***";
	qDebug() << "\t\t\t\t*** The Game has ended ***";
	qDebug() << "\t\t\t*** The Game has ended ***";
	qDebug() << "\t\t*** The Game has ended ***";
	qDebug() << "\t*** The Game has ended ***";
	qDebug() << "*** The Game has ended ***\n";

	timerGameTimeRemaining->stop();
	timerReHost->stop();
	timerBeacon->stop();

	if(gameInfo.getNumberOfPlayersInGame() == 0)	//There are no players in the game (they were all dumped)
	{
		setPromptText("Game Over");
		lttoComms->sendLCDtext(""					, 1, false);
		lttoComms->sendLCDtext("Game"               , 2, false);
		lttoComms->sendLCDtext("Over"				, 3,  true);

		ui->btn_Cancel->setEnabled(true);
		ui->btn_StartGame->setEnabled(false);

		//Show Scores window
		if(!scoresWindow) scoresWindow = new ScoresWindow(this);

		connect(scoresWindow,	SIGNAL(closingScoresWindow()),	this,	SLOT(closeHostGameWindow()) );

		scoresWindow->showFullScreen();

		return;
	}

	deBrief	= DeBrief::getInstance();

	connect(deBrief, SIGNAL(SendToHGWlistWidget(QString)), this, SLOT(InsertToListWidget(QString)) );
    timerDeBrief->start(DEBRIEF_TIMER_MSEC);

	currentPlayer = 1;                          // Set ready for DeBriefing to search for next player
    while (isThisPlayerHosted[currentPlayer] == false) currentPlayer++;
    ui->btn_StartGame->setEnabled(false);
    ui->btn_Rehost->setEnabled(false);
    ui->btn_Rehost->setVisible(false);
	// Why should this be active?
	//ui->btn_Cancel->setEnabled(true);
    ui->btn_SkipPlayer->setVisible(true);
    ui->btn_SkipPlayer->setEnabled(true);
    QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);
    setPromptText("DeBriefing " + teamNumText + playerInfoTemp[currentPlayer].getPlayerName());
    lttoComms->sendLCDtext("De"                                         , 1, false);
    lttoComms->sendLCDtext("Briefing"                                   , 2, false);
    lttoComms->sendLCDtext(teamNumText                                  , 3, false);
    lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName(), 4,  true);
	lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
    deBrief->prepareNewPlayerToDebrief(currentPlayer);
}

void HostGameWindow::deBriefTaggers()
{

////////////////////
//#define	LINEAR_DEBUG
////////////////////

#ifdef	LINEAR_DEBUG
	qDebug() << "HostGameWindow::deBriefTaggers() - Linear Debrief Mode";
	deBrief->checkIfPlayerIsDebriefed();
	if(deBrief->getIsPlayerDeBriefed())
	{
		// the current player has been debriefed
		currentPlayer++;
		while (isThisPlayerHosted[currentPlayer] == false) currentPlayer++;
		if(currentPlayer < 25)
		{
            QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);

            deBrief->prepareNewPlayerToDebrief(currentPlayer);
            setPromptText("Debriefing " + teamNumText + playerInfoTemp[currentPlayer].getPlayerName());
            lttoComms->sendLCDtext("De"                                         , 1, false);
            lttoComms->sendLCDtext("Briefing"                                   , 2, false);
            lttoComms->sendLCDtext(teamNumText                                  , 3, false);
            lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName(), 4,  true);
			lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);
		}
	}

	// If currentPlayer is valid - send the message
	if (currentPlayer < 25)
	{
		qDebug() << "HostGameWindow::deBriefTaggers() - Request Tag Reports for Player " << currentPlayer;
		deBrief->RequestTagReports();
	}
	else
		// All players are debriefed
	{
		currentPlayer = -1;
	}
#else
	//ReWork for non-linear deBriefing
	//This is required for large field games where Player1 might take
	//ten minutes to return and all the other players are waiting.

	qDebug() << "\t....(nonLinearDebriefing)...HostGameWindow::deBriefTaggers()";
	qDebug() << "\t    currentPlayer:" << currentPlayer;

	deBrief->checkIfPlayerIsDebriefed();

	if(deBrief->getIsPlayerDebriefing() == true)
	{
		qDebug() << "\t**** PlayerIsDebriefing .... I am outa here ! ****";
		return;
	}

	//if(deBrief->getIsPlayerDeBriefed() == false)
		if(true)
	{
		static uint8_t passCount = 0;
		if(passCount++ < 2)return;

		qDebug() << "\t........no reply to Debriefing request from Player:" <<
		currentPlayer++;
		while (isThisPlayerHosted[currentPlayer] == false) currentPlayer++;
		passCount = 0;
	}

	if (currentPlayer < 25)
	// If currentPlayer is valid - send the message
	{
        QString teamNumText     = "Team " + QString::number(((currentPlayer-1)/8)+1);

        deBrief->prepareNewPlayerToDebrief(currentPlayer);
        setPromptText("Debriefing " + teamNumText + " " + playerInfoTemp[currentPlayer].getPlayerName());
        lttoComms->sendLCDtext("De"                                         , 1, false);
        lttoComms->sendLCDtext("Briefing"                                   , 2, false);
        lttoComms->sendLCDtext(teamNumText                                  , 3, false);
        lttoComms->sendLCDtext(playerInfoTemp[currentPlayer].getPlayerName(), 4,  true);
		lttoComms->nonBlockingDelay(TEXT_SENT_DELAY);

		deBrief->RequestTagReports();

	}
	else if (deBrief->checkIfAllPlayersAreDebriefed() == false)
	// Go back around and try again for skipped players.
	{
		qDebug() << "Debriefing - start again";

		currentPlayer = 0;
	}
	else
	// All players are debriefed
	{
		qDebug() << "Debriefing - ALL DONE !!!!!!";
		currentPlayer = -1;
	}
#endif

	if(currentPlayer == -1)
	{
		// All players are debriefed
		timerDeBrief->stop();
		setPromptText("Finalising scores");
		lttoComms->sendLCDtext("All"                , 1, false);
		lttoComms->sendLCDtext("Players"            , 2, false);
		lttoComms->sendLCDtext("Reported"           , 3,  true);
		//Sit and wait to allow time for slow messages to appear
		lttoComms->nonBlockingDelay(1500);

		// Calculate scores and sendRankReport
		deBrief->calculateScores();
		deBrief->calculateRankings();

		//Show Scores window (which then sends the Rank Reports)
		if(!scoresWindow) scoresWindow = new ScoresWindow(this);

		connect(scoresWindow,	SIGNAL(closingScoresWindow()),	this,	SLOT(closeHostGameWindow()) );

		ui->btn_SkipPlayer->setEnabled(false);
		scoresWindow->showFullScreen();
	}
}

void HostGameWindow::disableHostSleep(bool state)
{
	qWarning() << "HostGameWindow::disableHostSleep() - " << state;
	qWarning() << "Nothing to see here !";
	//TODO: Add code for disabling sleep of host device. This will be OS specific.
}

void HostGameWindow::setPromptText(QString text)
{
	QString _textToSet = text;
	_textToSet.prepend("<html><head/><body><p><span style= font-size:64pt; font-weight:600;><b>");
	_textToSet.append("</span></p></body></b></html>");
	ui->label_Prompt->setText(_textToSet);
}

void HostGameWindow::setNextPlayerText(QString text)
{
	QString _textToSet = text;
	_textToSet.prepend("<html><head/><body><p><span style= font-size:32pt; font-weight:600;><b>");
	_textToSet.append("</span></p></body></b></html>");
	ui->label_NextPlayer->setText(_textToSet);
}

void HostGameWindow::on_showLog_clicked()
{
	ui->listWidget_Status->setVisible(true);
}

void HostGameWindow::UpdateBatteryDisplay(QString volts)
{
	if(firmWareVersionReceived != true) ui->label_FirmwareStatus->setText("<html><head/><body><p><span style= color:#fc0107; font-size:18pt;>The Base station firmware is out of date.</span></p></body></html>");

	//qDebug() << "HostGameWindow::UpdateBatteryDisplay() - " << volts.toFloat();
	if(volts.endsWith("@")) volts.remove(4, 2);

	if (volts.length() > 5)
	{
		qDebug() << "\t***** HostGameWindow::UpdateBatteryDisplay() FAULT !!!!!!! -" << volts;
		return;
	}
	if(volts.toFloat() < LOW_BATT_LEVEL)
	{
		//ui->label_BattVolts->setText("<html><span style = font-size:18pt; color:#FF0000>Low Battery = " + volts + "v </span></html>");
		ui->label_BattVolts->setText("<html><head/><body><p><span style= color:#fc0107; font-size:18pt;>Low Battery = " + volts + " v</span></p></body></html>");

	}
	else
	{
		ui->label_BattVolts->setText("<html><span style = font-size:18pt;>Battery = " + volts + "v </span></html>");
	}

}

void HostGameWindow::resetScores()
{
	qDebug() << "HostGameWindow::resetScores()";
	for(int index = 1; index <= MAX_PLAYERS; index++)
	{
		playerInfo[index].setGameScore(0);
//		if(gameInfo.getCumulativeScoreMode() == false)
//		{
//			for (int subIndex = 0; subIndex <= MAX_PLAYERS; subIndex++)
//			{
//				playerInfo[index].setTagsTaken(subIndex, 0);
//			}
//			playerInfo[index].setZoneTimeMinutes(0);
//			playerInfo[index].setZoneTimeSeconds(0);
//			playerInfo[index].setSurvivalTimeMinutes(0);
//			playerInfo[index].setSurvivalTimeSeconds(0);
//		}
	}
}

void HostGameWindow::removePhantomTeam3players()
{
	if(gameInfo.getNumberOfTeams() == 2)
		{
		qDebug() << "HostGameWindow::removePhantomTeam3players()";
		for(int index = 17; index <= MAX_PLAYERS; index++)
			{
				if(gameInfo.getIsThisPlayerInTheGame(index))
				{
					gameInfo.setIsThisPlayerInTheGame(index, false);
					qDebug() << "HostGameWindow::removePhantomTeam3players() - Removing player" << index;
				}
			}
		}
}

void HostGameWindow::sendGameSettingsToLog()
{
	qInfo() << endl;
	qInfo() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
	qInfo() << "HostGameWindow::sendGameSettingsToLog()";
	if (gameInfo.getIsLTARGame())	qInfo() << "This is an LTAR mode game";
	else							qInfo() << "This is an normal mode game";
	if (gameInfo.getCumulativeScoreMode())	qInfo() << "Scores are cumultative";
	else									qInfo() << "Scores reset with each game";
	qInfo() << "GameType:\t\t"				<< gameInfo.getGameType() << gameInfo.getGameName();
	qInfo() << "Number of teams:\t"		<< gameInfo.getNumberOfTeams();
	qInfo() << "Number of players:\t"	<< gameInfo.getNumberOfPlayersInGame();
	qInfo() << "Number of Spies:\t"		<< gameInfo.getNumberOfSpies();
	if (gameInfo.getIsSpiesTeamTagActive())	qInfo() << "Spies have TeamTags Active";
	else									qInfo() << "Spies have TeamTags Disabled";
	if(gameInfo.getIsReSpawnGame())			qInfo() << "Respwan is Active";
	else									qInfo() << "Respawn is Disabled";
	if(playerInfo[0].getMedicMode())		qInfo() << "Global MedicMode is Active";
	else									qInfo() << "Global MedicMode is Disabled";
	if(playerInfo[0].getTeamTags())			qInfo() << "Global TeamTags are Active";
	else									qInfo() << "Global TeamTags are Disabled";
	if(playerInfo[0].getSlowTags())			qInfo() << "Global SlowTags are Active";
	else									qInfo() << "Global SlowTags are Disabled";
	qInfo() << "Global Flags 1:  " << QString::number(playerInfo[0].getPackedFlags1(), 2);
	qInfo() << "Global Flags 2:  " << QString::number(playerInfo[0].getPackedFlags2(), 2);
	if(gameInfo.getIsLTARGame())  qInfo() << "Global Flags 3:  " << QString::number(playerInfo[0].getPackedFlags3(), 2);
	for (int index = 1; index <= MAX_PLAYERS; index++)
	{
		if(gameInfo.getIsThisPlayerInTheGame(index))
		{
			qInfo() << "     Player" << index << "-" << playerInfo[index].getPlayerName();
			qInfo() << "\tMedicMode:\t" <<	playerInfo[index].getMedicMode();
			qInfo() << "\tTeamTags:\t" <<	playerInfo[index].getTeamTags();
			qInfo() << "\tSlowTags:\t" <<	playerInfo[index].getSlowTags();
			qInfo() << "\tFlags 1:\t" << QString::number(playerInfo[index].getPackedFlags1(), 2);
			qInfo() << "\tFlags 2:\t" << QString::number(playerInfo[index].getPackedFlags2(), 2);
			if(gameInfo.getIsLTARGame())  qInfo() << "\tFlags 3:\t" << QString::number(playerInfo[index].getPackedFlags3(), 2);
		}
	}
	qInfo() << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
	qInfo() << endl;
}

void HostGameWindow::checkFirmwareVersion(QString fWareVersion)
{
	firmWareVersionReceived = true;
	qDebug() << "HostGameWindow::checkFirmwareVersion()  Firmware = " << fWareVersion;
	fWareVersion.remove(4,2);
	qDebug() << "Truncated version =" << fWareVersion.toDouble() << CURRENT_BASESTATION_FIRMWARE;
	if(fWareVersion.toDouble() < CURRENT_BASESTATION_FIRMWARE)	ui->label_FirmwareStatus->setText("<html><head/><body><p><span style= color:#fc0107; font-size:18pt;>The Base station firmware is out of date.</span></p></body></html>");
    else														ui->label_FirmwareStatus->setText("");
}



