#include "HostGameWindow.h"
#include "ui_HostGameWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QApplication>
#include "Defines.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"

#include <QInputDialog>

int playerDebugNum = 0;

HostGameWindow::HostGameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostGameWindow)
{
    ui->setupUi(this);

    timerAnnounce           = new QTimer(this);
    timerCountDown          = new QTimer(this);
    timerDeBrief            = new QTimer(this);
    timerAssignFailed       = new QTimer(this);
    timerGameTimeRemaining  = new QTimer(this);

    connect(timerAnnounce,          SIGNAL(timeout() ),             this, SLOT(announceGame())            );
    connect(timerCountDown,         SIGNAL(timeout() ),             this, SLOT(sendCountDown())           );
    connect(timerDeBrief,           SIGNAL(timeout() ),             this, SLOT(deBriefTaggers())          );
    connect(timerAssignFailed,      SIGNAL(timeout() ),             this, SLOT(sendAssignFailed())        );
    connect(timerGameTimeRemaining, SIGNAL(timeout() ),             this, SLOT(updateGameTimeRemaining()) );
    connect(&lttoComms,      SIGNAL(RequestJoinGame(int,int,int) ), this, SLOT(AssignPlayer(int,int,int)) );
    connect(&lttoComms,      SIGNAL(AckPlayerAssignment(int,int) ), this, SLOT(AddPlayerToGame(int,int))  );
    connect(&serialUSBcomms, SIGNAL(SerialPortFound(QString)),      this, SLOT(AddSerialPortToListWidget(QString)) );

    timerAnnounce->start(HOST_TIMER_MSEC);

    for (int x = 0; x<25;x++) isThisPlayerHosted[x] = false;
    currentPlayer = 1;
    noMorePlayers = false;
    playerDebugNum = 0;
    closingWindow = false;
    sendingCommsActive = false;
    countDownTimeRemaining = DEFAULT_COUNTDOWN_TIME;
    ui->btn_Cancel->setText("Cancel");
    ui->btn_Rehost->setEnabled(false);

    //TODO: finish rehost and delete this line
    ui->btn_Rehost->setVisible(false);

    serialUSBcomms.setUpSerialPort();
}

HostGameWindow::~HostGameWindow()
{
    //qDebug() << "HostGameWindow::~HostGameWindow() - Triggered";
    timerAnnounce->stop();
    timerCountDown->stop();
    timerDeBrief->stop();
    timerAssignFailed->stop();
    timerGameTimeRemaining->stop();
    qDebug() << "HostGameWindow::~HostGameWindow() - Shutting Down !";
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
    if (sendingCommsActive == false) deleteLater();     //If this is true then the deleteLater is triggered at the end of hostCurrentPlayer(), to stop the app crashing.
}

////////////////////////////////////////////////////////////////////////////////

void HostGameWindow::SetAnnounceTimerBlock(bool state)
{
    timerAnnounce->blockSignals(state);
}

////////////////////////////////////////////////////////////////////////////////

void HostGameWindow::announceGame()
{
    sendingCommsActive = true;

    //Set Base Station LED status
    if (lttoComms.getTcpCommsConnected() == false)  ui->led_Status->setStyleSheet("background-color: yellow; border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey;");
    else
    {
        ui->led_Status->setStyleSheet("background-color: rgb(0,255,50); border-style: outset; border-width: 2px; border-radius: 10px; border-color: grey;");
        lttoComms.setDontAnnounceGame(false);
    }


    if (currentPlayer != 0)                                                                 // Player 0 is the dummy player
    {
        while (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false) currentPlayer++;
        ui->label->setText("Prepare to Host : Tagger " + QString::number(currentPlayer));
        if (playerInfo[currentPlayer].getPlayerName() != "") ui->label->setText("Prepare to Host : " + playerInfo[currentPlayer].getPlayerName());
    }
    if (currentPlayer > 16 && gameInfo.getNumberOfTeams() == 2) currentPlayer = 25;         // Ignore players in Team 3
    if (currentPlayer >24)                                                                  // No more players to add
    {
        ui->label->setText("All players are hosted, press START ");
        ui->btn_StartGame->setEnabled(true);
        currentPlayer = 0;      //Transmits global game data to keep guns sync'd
    }

    hostCurrentPlayer();
}


void HostGameWindow::hostCurrentPlayer()
{

    if(lttoComms.getDontAnnounceGame() == true )
    {
        lttoComms.setMissedAnnounceCount(lttoComms.getMissedAnnounceCount()+1);
        qDebug() << "HostGameWindow::hostCurrentPlayer() - Missed count = " << lttoComms.getMissedAnnounceCount();
        if (lttoComms.getMissedAnnounceCount() < 3) return;

        else if (expectingAckPlayerAssignment == true)   // we appear to have missed the AckPlayerAssignment message.
        {
            qDebug() << "HostGameWindow::hostCurrentPlayer() - assignPlayerFailed !!"  ;
            assignPlayerFailed();
        }
    }

    //Change TeamTags if required for Spies.
    bool wereCurrentPlayerTeamTagsActive = playerInfo[currentPlayer].getTeamTags();
    if(gameInfo.getIsSpiesTeamTagActive() && gameInfo.getNumberOfSpies() > 0 && gameInfo.getNumberOfTeams() > 1)
    {
        playerInfo[currentPlayer].setTeamTags(true);
    }

    if (lttoComms.getTcpCommsConnected() == true || serialUSBcomms.getSerialCommsConnected() == true) InsertToListWidget("Announce Game : Player " + QString::number(currentPlayer));
    else
    {
        InsertToListWidget("Trying to connect to Base Station");
        ui->label->setText("Connecting to Base Station......");
    }
    //qDebug() << "\tHostGameWindow::hostCurrentPlayer() - Player:" << currentPlayer;

    lttoComms.sendPacket(PACKET, gameInfo.getGameType()                        );
    lttoComms.sendPacket(DATA,   gameInfo.getGameID()                          );
    lttoComms.sendPacket(DATA,   gameInfo.getGameLength(),                  BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getHealthTags()), BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads()),    BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getShieldTime()), BCD);
    lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()),   BCD);

    //  Set the Reloads flag to match handicapped values
    if      (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads())  <  100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, true);
    }
    else if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getReloads())  == 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_RELOADS_FLAG, false);
    }

    //  Set the MegaTags flag to match handicapped values
    if      (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()) < 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_MEGAS_FLAG,   true);
    }
    else if (playerInfo[currentPlayer].handicapAdjust(playerInfo[currentPlayer].getMegaTags()) == 100)
    {
        playerInfo[currentPlayer].setBitFlags1(LIMITED_MEGAS_FLAG,   false);
    }

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
    lttoComms.sendPacket(CHECKSUM);

    //If the current Player did not have TeamTags active, turn them off again. (altered if they are a Spy)
    if(wereCurrentPlayerTeamTagsActive == false) playerInfo[currentPlayer].setTeamTags(false);

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
        qDebug() << "Flag1 = " << playerInfo[currentPlayer].getPackedFlags1() << " Flag2 = "<< playerInfo[currentPlayer].getPackedFlags2();

 //TODO: Remove this, Triggers deBug messgaes for testing only
 calculatePlayerTeam5bits(currentPlayer);
 //TODO:

    }

    sendingCommsActive = false;
    if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
    return;
}

int assignPlayerFailCount;
bool dontAnnounceFailedSignal;

void HostGameWindow::assignPlayerFailed()
{
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

void HostGameWindow::sendAssignFailed()
{
    if(dontAnnounceFailedSignal == false)
    {
        assignPlayerFailCount++;

        lttoComms.sendPacket(PACKET, ASSIGN_PLAYER_FAIL                      );
        lttoComms.sendPacket(DATA,   gameInfo.getGameID()                    );
        lttoComms.sendPacket(DATA,   playerInfo[currentPlayer].getTaggerID() );
        lttoComms.sendPacket(CHECKSUM                                        );
         qDebug() << "HostGameWindow::sendAssignFailed()  - Sending # " << assignPlayerFailCount;
    }

    if (assignPlayerFailCount == 6)   //give up and start again
    {
        isThisPlayerHosted[currentPlayer] = false;
        playerInfo[currentPlayer].setTaggerID(0);
        timerAssignFailed->stop();
        lttoComms.setDontAnnounceGame(false);
        expectingAckPlayerAssignment = false;
        qDebug() <<"HostGameWindow::sendAssignFailed() - Counted to 6, I am now going away";
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
        // If I do then the setup is trickier (I think.....)
    }

    if (gameInfo.getNumberOfTeams() == 0)   //Players 1 to 24, no teams
    {
        assignedTeamNumber = 0;
        assignedPlayerNumber = currentPlayer +7;                    // For solo games: zero-based player number + 8
        playerTeam5bits = 8 + (currentPlayer-1);
        return playerTeam5bits;
    }
    else
    {
       //TODO: Move this to a separate function.

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


void HostGameWindow::AssignPlayer(int Game, int Tagger, int Flags)

{
    InsertToListWidget("   AssignPlayer()" + QString::number(currentPlayer) + ", Tagger ID:" + QString::number(Tagger) );
    qDebug() << "   AssignPlayer()" << currentPlayer << ", Tagger ID:" << Tagger;

    //Check if the gameID from the tagger is correct.
    if(gameInfo.getGameID() != Game) return;

    int preferedTeam = Flags & 3;  //b00000011;       //This is the 2 LSB of the flags byte.

    if(isThisPlayerHosted[currentPlayer] == false)
    {
        lttoComms.setDontAnnounceGame(true);
        playerInfo[currentPlayer].setTaggerID(Tagger);

        lttoComms.sendPacket(PACKET,  ASSIGN_PLAYER);
        lttoComms.sendPacket(DATA,    Game);
        lttoComms.sendPacket(DATA,    Tagger);
        lttoComms.sendPacket(DATA,    calculatePlayerTeam5bits(preferedTeam) );
        lttoComms.sendPacket(CHECKSUM);

        expectingAckPlayerAssignment = true;

        qDebug() << "HostGameWindow::AssignPlayer() " << currentPlayer << Game << Tagger, calculatePlayerTeam5bits(Flags);
        if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
    }
}

void HostGameWindow::AddPlayerToGame(int Game, int Tagger)
{    
    qDebug() << "HostGameWindow::AddPlayerToGame()" << currentPlayer;

    if(gameInfo.getGameID() != Game || playerInfo[currentPlayer].getTaggerID() != Tagger) return;
    InsertToListWidget("   AddPlayerToGame()" + QString::number(currentPlayer));
    isThisPlayerHosted[currentPlayer] = true;
    if (currentPlayer != 0) currentPlayer++;
    lttoComms.setDontAnnounceGame(false);
    expectingAckPlayerAssignment = false;
    timerAssignFailed->stop();
    if (closingWindow) deleteLater();   // Delete the window, as the cancel button has been pressed.
}

void HostGameWindow::AddSerialPortToListWidget(QString value)
{
    QString portFound = QObject::tr("") + value;
    qDebug() << portFound;
    InsertToListWidget(portFound);
}

void HostGameWindow::on_btn_Start_clicked()     //DeBug only. Remove it later.
{
    //This is the Start/Stop (Debug) button on the form.
    announceGame();
    if (ui->btn_Start->isChecked()) timerAnnounce->start(HOST_TIMER_MSEC);
    else timerAnnounce->stop();
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
        timerGameTimeRemaining->stop();
        timerDeBrief->start(HOST_TIMER_MSEC);
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Rehost->setEnabled(false);
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
    if(countDownTimeRemaining == 0)
    {
        timerCountDown->stop();
        timerGameTimeRemaining->start(1000);
        remainingGameTime = (ConvertBCDtoDec(gameInfo.getGameLength()) * 60) + 2;  // Add a couple of seconds to match the taggers, who start the clock AFTER the Good Luck message.

        InsertToListWidget("Game has started !!!");
        ui->label->setText("Game Underway !!!");
        ui->btn_Cancel->setText("Close");
        ui->btn_Rehost->setEnabled(true);
        ui->btn_StartGame->setText("End\nGame");
        ui->btn_StartGame->setEnabled(true);
        //TODO:Change the form to show the DeBrief stuff - use a State Machine?
    }
    else
    {
        lttoComms.sendPacket(PACKET , COUNTDOWN);
        lttoComms.sendPacket(DATA, gameInfo.getGameID() );
        lttoComms.sendPacket(DATA, ConvertDecToBCD(countDownTimeRemaining));
        lttoComms.sendPacket(DATA, 8);   //TODO:Team1 PlayerCount
        lttoComms.sendPacket(DATA, 8);   //TODO:Team2 PlayerCount
        lttoComms.sendPacket(DATA, 8);   //TODO:Team3 PlayerCount
        lttoComms.sendPacket(CHECKSUM);
        InsertToListWidget("CountDownTime = " + QString::number(countDownTimeRemaining));
        ui->label->setText("CountDownTime = " + QString::number(countDownTimeRemaining));
        countDownTimeRemaining--;
    }
}

void HostGameWindow::updateGameTimeRemaining()
{
    remainingGameTime--;
    if (remainingGameTime > (ConvertBCDtoDec(gameInfo.getGameLength()) * 60)) return;   // Ignore the extra 2 seconds we added to the clock.
    QString remainingMinutes = QString::number(remainingGameTime / 60);
    QString remainingSeconds = QString::number(remainingGameTime % 60);
    if (remainingMinutes.length() == 1) remainingMinutes.prepend("0");
    if (remainingSeconds.length() == 1) remainingSeconds.prepend("0");
    ui->label->setText("Game Time Remaining  " + remainingMinutes + ":" + remainingSeconds);
    if (remainingGameTime == 0)
    {
        timerGameTimeRemaining->stop();
        timerDeBrief->start(HOST_TIMER_MSEC);
        ui->btn_StartGame->setEnabled(false);
        ui->btn_Rehost->setEnabled(false);
        ui->label->setText("DeBriefing..... but not just yet :-)");
    }
}

void HostGameWindow::deBriefTaggers()
{
    lttoComms.sendPacket(PACKET , REQUEST_TAG_REPORT);
    lttoComms.sendPacket(DATA, gameInfo.getGameID() );
    lttoComms.sendPacket(DATA, 0x20 );    // Team and player number (8 bits) - Team number (4 bits) one-based, Player number (4 bits) zero-based
    lttoComms.sendPacket(DATA, 0x0F );
    lttoComms.sendPacket(CHECKSUM);
}

int HostGameWindow::ConvertDecToBCD(int dec)
{
  if (dec == 0xFF) return dec;
  return (int) (((dec/10) << 4) | (dec %10) );
}

int HostGameWindow::ConvertBCDtoDec(int bcd)
{
  if (bcd == 0xFF) return bcd;
  return (int) (((bcd >> 4) & 0xF) *10) + (bcd & 0xF);
}

void HostGameWindow::on_btn_SkipPlayer_clicked()
{
    currentPlayer++;
    lttoComms.setDontAnnounceGame(false);
}

void HostGameWindow::InsertToListWidget(QString lineText)
{
    ui->listWidget_Status->addItem(lineText);
    if (ui->listWidget_Status->count() > 18) ui->listWidget_Status->takeItem(0);
}

void HostGameWindow::on_btn_TestMessage_clicked()
{

    lttoComms.sendPacket(TAG,  0x00);
    return;

    //P80:D48:D45:D4C:D4C:D4F:CF4:
    lttoComms.sendPacket(PACKET,  0x80);
    lttoComms.sendPacket(DATA,    0x48);
    lttoComms.sendPacket(DATA,    0x45);
    lttoComms.sendPacket(DATA,    0x4C);
    lttoComms.sendPacket(DATA,    0x4C);
    lttoComms.sendPacket(DATA,    0x4F);
    lttoComms.sendPacket(CHECKSUM     );
    return;
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

void HostGameWindow::on_btn_Rehost_clicked()
{
    reHostTagger = new ReHostTagger(this);
    reHostTagger->show();
}

void HostGameWindow::on_btn_FailSend_clicked()
{
    lttoComms.setDontAnnounceGame(true);
    sendAssignFailed();
}

void HostGameWindow::on_btn_SetFlags2_clicked()
{
    bool ok;
    int Flags2 = QInputDialog::getInt(this, "Set Flags2", "Enter a number:", playerInfo[currentPlayer].getPackedFlags2(), 0, 255, 1, &ok);
    if(ok)
    {
        for (int x = 1; x < 25; x++)
        {
            playerInfo[x].setPackedFlags2(Flags2);
        }

        //playerInfo[currentPlayer].setPackedFlags2(Flags2);
    }
}
