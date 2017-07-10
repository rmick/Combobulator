#include "HostGameWindow.h"
#include "ui_HostGameWindow.h"
#include <QMessageBox>
#include <QDebug>
#include "Defines.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"

HostGameWindow::HostGameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostGameWindow)
{
    ui->setupUi(this);

    timerAnnounce   = new QTimer(this);
    timerCountDown  = new QTimer(this);

    connect(timerAnnounce,  SIGNAL(timeout() ),                     this, SLOT(announceGame()) );
    connect(timerCountDown, SIGNAL(timeout() ),                     this, SLOT(sendCountDown()) );
    connect(&lttoComms,     SIGNAL(RequestJoinGame(int,int,int) ),  this, SLOT(AssignPlayer(int,int,int)) );
    connect(&lttoComms,     SIGNAL(AckPlayerAssignment(int,int) ),  this, SLOT(AddPlayerToGame(int,int)) );
    connect(&serialUSBcomms, SIGNAL(SerialPortFound(QString)),      this, SLOT(AddSerialPortToListWidget(QString)) );

    timerAnnounce->start(HOST_TIMER_MSEC);

    #ifdef Q_OS_ANDROID
        InsertToListWidget("This is Android");
    #endif

    for (int x = 0; x<25;x++) isThisPlayerHosted[x] = false;
    currentPlayer = 1;
    noMorePlayers = false;
    countDownTimeRemaining = DEFAULT_COUNTDOWN_TIME;
    ui->btn_Cancel->setText("Cancel");
    ui->btn_Rehost->setEnabled(false);

    serialUSBcomms.setUpSerialPort();

    // Kick start the process, otherwise there is a 2.0s delay until the timer first triggers.
    announceGame();
}

HostGameWindow::~HostGameWindow()
{
    timerAnnounce->stop();
    timerCountDown->stop();
    #ifdef  INCLUDE_SERIAL_USB
        serialUSBcomms.closeSerialPort();
    #endif
    delete ui;
}

void HostGameWindow::hideEvent(QHideEvent *)
{
    timerAnnounce->stop();
    timerCountDown->stop();
    #ifdef  INCLUDE_SERIAL_USB
        serialUSBcomms.closeSerialPort();
    #endif
}

void HostGameWindow::on_btn_Cancel_clicked()
{
    timerAnnounce->stop();
    timerCountDown->stop();
    #ifdef  INCLUDE_SERIAL_USB
        serialUSBcomms.closeSerialPort();
    #endif
    deleteLater();
}

////////////////////////////////////////////////////////////////////////////////

void HostGameWindow::SetAnnounceTimerBlock(bool state)
{
    timerAnnounce->blockSignals(state);
}

////////////////////////////////////////////////////////////////////////////////

void HostGameWindow::announceGame()
{
    //if (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false)  // DO I add this in?? It stops reuse of player numbers, but is needed if I use the requested team flag  // && isThisPlayerHosted[currentPlayer] == false)
    //{
        if (currentPlayer != 0)
        {
            while (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false) currentPlayer++;
            ui->label->setText("Prepare to Host Tagger : " + QString::number(currentPlayer));
        }
        if (currentPlayer >24)
        {
            ui->label->setText("All players are hosted, press START ");
            ui->btn_StartGame->setEnabled(true);
            currentPlayer = 0;
        }
    //}
    hostCurrentPlayer();
}


int playerDebugNum = 0;
void HostGameWindow::hostCurrentPlayer()
{

    if(lttoComms.getDontAnnounceGame() == true )
    {
        lttoComms.setMissedAnnounceCount(lttoComms.getMissedAnnounceCount()+1);
        qDebug() << "HostGameWindow::hostCurrentPlayer() - Missed count = " << lttoComms.getMissedAnnounceCount();
        if (lttoComms.getMissedAnnounceCount() < 3) return;
        else lttoComms.setDontAnnounceGame(false);
    }

    InsertToListWidget("Announce Game : Player " + QString::number(currentPlayer));
    qDebug() << "";
    qDebug() << "\tHostGameWindow::hostCurrentPlayer() - GameID:" + QString::number(gameInfo.getGameID());

    if(playerInfo[currentPlayer].getSpyNumber() != 0)
    {
        qDebug() << "HostGameWindow::hostCurrentPlayer() - This is a SPY !!!";
    }

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
    }




    return;
}

int HostGameWindow::calculatePlayerTeam5bits(int requestedTeam)
{
    int playerTeam5bits = 0;

    if(requestedTeam !=0)   //
        ;                   //To silence compiler warning.

    //qDebug() << "HostGameWindow::calculatePlayerTeam5bit() - Requested Team = " << requestedTeam;

    if (gameInfo.getNumberOfTeams() == 0)   //Players 1 to 24, no teams
    {
        playerTeam5bits = 8 + (currentPlayer-1);
    }
    else                                    //2 bits for Team, 3 bits for PlayerID
    {
        if      (currentPlayer > 0  && currentPlayer < 9)
        {
            playerTeam5bits = 1 << 3;
            playerTeam5bits += (currentPlayer - 1);
        }
        else if (currentPlayer > 8  && currentPlayer < 17)
        {
            playerTeam5bits = 2 << 3;
            playerTeam5bits += (currentPlayer - 9);
        }
        else if (currentPlayer > 18 && currentPlayer < 24)
        {
            playerTeam5bits = 3 << 3;
            playerTeam5bits += (currentPlayer - 17);
        }
    }
    //qDebug() << "   HostGameWindow::calculatePlayerTeam5bits()"  << currentPlayer << displayBinary(playerTeam5bits, 8);
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


        bool expectingAckPlayerAssignment = true;

        qDebug() << "HostGameWindow::AssignPlayer() " << currentPlayer << Game << Tagger, calculatePlayerTeam5bits(Flags);
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
}

void HostGameWindow::AddSerialPortToListWidget(QString value)
{
    QString portFound = QObject::tr("") + value;
    qDebug() << portFound;
    InsertToListWidget(portFound);
}

void HostGameWindow::on_btn_Start_clicked()
{
    announceGame();
    if (ui->btn_Start->isChecked()) timerAnnounce->start(2000);
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

void HostGameWindow::resetPlayersForNewGame()
{
    currentPlayer = 1;
    for(int players= 0; players < 25; players++)
    {
        isThisPlayerHosted[players] = false;
    }
    noMorePlayers = false;
    assignSpies();
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
    else
    {
        //SendEndGame();
        //ui->btn_StartGame->setText("Debrief");
        ui->btn_StartGame->setText("Start\nthe\nGame");


    }

}

void HostGameWindow::sendCountDown()
{
    if(countDownTimeRemaining == 0)
    {
        timerCountDown->stop();
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
        countDownTimeRemaining--;
    }
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


//    const int Game = 0;

//        ////--------------------------------------------------
//        //  Host 2TeamRespawn Roonka Game (captured packets from LTTO host)     - WORKS - Tagger screen shows 2TRS
//        QString hostedGameType         = "0C";
//        QString hostedGameID           = "42";
//        QString hostedGameTime         = "15";
//        QString hostedTagsAvailable    = "99";
//        QString hostedReloadsAvailable = "FF";
//        QString hostedShieldTime       = "60";
//        QString hostedMegaTags         = "15";
//        QString hostedPackedFlags1     = "A8";
//        QString hostedPackedFlags2     = "32";
//        QString name1                  = "32";
//        QString name2                  = "54";
//        QString name3                  = "52";
//        QString name4                  = "53";
//        QString checkSum               = "00";

//        if      (Game == 0) qDebug() << endl << "Sending Roonka Game";

//        else if (Game ==1)
//        {
//            qDebug() << endl << "Sending Patent Game";
//            ////--------------------------------------------------
//            //  Game info from Patent                                           - WORKS - Tagger screen shows 2ZON
//            hostedGameType         = "0C";  //HEX
//            hostedGameID           = "2C";  //HEX
//            hostedGameTime         = "15";  //DEC
//            hostedTagsAvailable    = "50";  //DEC
//            hostedReloadsAvailable = "FF";  //DEC
//            hostedShieldTime       = "45";  //DEC
//            hostedMegaTags         = "12";  //DEC
//            hostedPackedFlags1     = "28";  //HEX
//            hostedPackedFlags2     = "A2";  //HEX
//            name1                  = "32";  //HEX
//            name2                  = "5A";  //HEX
//            name3                  = "4F";  //HEX
//            name4                  = "4E";  //HEX
//            checkSum               = "ZZ";
//            //checkSum               = "E6";  //HEX
//        }

//        else if (Game ==2)
//        {
//            qDebug() << endl << "Sending LazerSwarm Game";
//            ////--------------------------------------------------
//            //  Game info from Patent                                           - WORKS - Tagger responds with RequestJoinGame
//            hostedGameType         = "02";  //HEX
//            hostedGameID           = "7C";  //HEX
//            hostedGameTime         = "10";  //DEC
//            hostedTagsAvailable    = "10";  //DEC
//            hostedReloadsAvailable = "FF";  //DEC
//            hostedShieldTime       = "15";  //DEC
//            hostedMegaTags         = "10";  //DEC
//            hostedPackedFlags1     = "20";  //HEX
//            hostedPackedFlags2     = "01";  //HEX
//            name1                  = "00";  //HEX
//            name2                  = "00";  //HEX
//            name3                  = "00";  //HEX
//            name4                  = "00";  //HEX
//            checkSum               = "ZZ";
//            //checkSum               = "E3";  //HEX
//        }

//        else if (Game ==3)
//        {
//            qDebug() << endl << "Sending LTTO default Game";
//            ////--------------------------------------------------
//            //  Game info from Patent                                               - WORKS - Tagger responds with RequestJoinGame
//            hostedGameType         = "02";  //HEX
//            hostedGameID           = "A1";  //HEX
//            hostedGameTime         = "10";  //DEC
//            hostedTagsAvailable    = "10";  //DEC
//            hostedReloadsAvailable = "FF";  //DEC
//            hostedShieldTime       = "15";  //DEC
//            hostedMegaTags         = "10";  //DEC
//            hostedPackedFlags1     = "20";  //HEX
//            hostedPackedFlags2     = "01";  //HEX
//            name1                  = "00";  //HEX
//            name2                  = "00";  //HEX
//            name3                  = "00";  //HEX
//            name4                  = "00";  //HEX
//            checkSum               = "ZZ";
//            //checkSum               = "08";  //HEX
//        }

//        else if (Game ==4)
//        {
//            qDebug() << endl << "Sending Copy of saved LazerSwarm";
//            ////--------------------------------------------------
//            //  Game info from Patent                                               - NoGo !
//            hostedGameType         = "02";
//            hostedGameID           = "3B";
//            hostedGameTime         = "10";
//            hostedTagsAvailable    = "0A";
//            hostedReloadsAvailable = "FF";
//            hostedShieldTime       = "0F";  //DEC
//            hostedMegaTags         = "0A";  //DEC
//            hostedPackedFlags1     = "14";  //HEX
//            hostedPackedFlags2     = "01";  //HEX
//            name1                  = "00";  //HEX
//            name2                  = "00";  //HEX
//            name3                  = "00";  //HEX
//            name4                  = "00";  //HEX
//            checkSum               = "ZZ";
//            //checkSum               = "08";  //HEX
//        }


//        serialComms.sendPacket(PACKET,  hostedGameType);
//        serialComms.sendPacket(DATA,    hostedGameID);
//        serialComms.sendPacket(DATA,    hostedGameTime);
//        serialComms.sendPacket(DATA,    hostedTagsAvailable);
//        serialComms.sendPacket(DATA,    hostedReloadsAvailable);
//        serialComms.sendPacket(DATA,    hostedShieldTime);
//        serialComms.sendPacket(DATA,    hostedMegaTags);
//        serialComms.sendPacket(DATA,    hostedPackedFlags1);
//        serialComms.sendPacket(DATA,    hostedPackedFlags2);
//        if (hostedGameType == 0x0C)
//        {
//            serialComms.sendPacket(DATA, name1);
//            serialComms.sendPacket(DATA, name2);
//            serialComms.sendPacket(DATA, name3);
//            serialComms.sendPacket(DATA, name4);
//        }
//        serialComms.sendPacket(CHECKSUM, 0);
}

void HostGameWindow::assignSpies()
{
    InsertToListWidget("HostGameWindow::assignSpies()" + QString::number(gameInfo.getNumberOfSpies()));

    //Assign all players Spy = 0
    for (int x = 0; x < 25; x++)
    {
        playerInfo[x].setSpyNumber(0);
    }

    if(gameInfo.getNumberOfSpies() == 0) return;

    //add some checking so that NumberOfSpies is !> number of players in any team.
    int numberOfPlayersInTeam1 = 0;
    int numberOfPlayersInTeam2 = 0;
    int numberOfPlayersInTeam3 = 0;
    for (int x = 1; x < 9; x++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(x))       numberOfPlayersInTeam1++;
        if (gameInfo.getIsThisPlayerInTheGame(x+8))     numberOfPlayersInTeam2++;
        if (gameInfo.getIsThisPlayerInTheGame(x+16))    numberOfPlayersInTeam3++;
    }
    if(     (gameInfo.getNumberOfSpies() *2) >= numberOfPlayersInTeam1 ||
            (gameInfo.getNumberOfSpies() *2) >= numberOfPlayersInTeam2 ||
            (gameInfo.getNumberOfSpies() *2) >= numberOfPlayersInTeam3  )
            {
                int action = QMessageBox::warning(this,tr("Are you sure?"), tr("More than 50% of your players are spies. This will make the game <i>'interesting'</i> ! \nPress OK to continue, or Cancel to set Spies to Zero."), QMessageBox::Ok | QMessageBox::Cancel);
                if (action == QMessageBox::Cancel) gameInfo.setNumberOfSpies(0);
            }

    //assign that player as Spy in each time (Spy = 1, Spy = 2)
    int randomNumber;
    for (int s = 1; s <= gameInfo.getNumberOfSpies(); s++)
    {
        bool loop = true;
        while (loop == true)
        {
            randomNumber = GetRandomnumber(1, 8);
            qDebug() <<"Random Number is " << randomNumber;

            if (playerInfo[randomNumber].getSpyNumber() == 0)
            {
                playerInfo[randomNumber   ].setSpyNumber(s);
                playerInfo[randomNumber+8 ].setSpyNumber(s);
                playerInfo[randomNumber+16].setSpyNumber(s);
                if(gameInfo.getIsSpiesTeamTagActive() )
                {
                    playerInfo[randomNumber   ].setTeamTags(true);
                    playerInfo[randomNumber+8 ].setTeamTags(true);
                    playerInfo[randomNumber+16].setTeamTags(true);
                }
                loop = false;
            }
        }
    }

    //Check this value when hosting a player, if (Spy != 0) swap teams.
    //Not sure how to do 3 teams yet, 1>2,2>3,3>1
}

void HostGameWindow::OpenPorts()
{

}

void HostGameWindow::ClosePorts()
{

}

int HostGameWindow::GetRandomnumber(int min, int max)
{
    return ((qrand() % ((max + 1) - min)) + min);
}

void HostGameWindow::on_btn_Rehost_clicked()
{
    reHostTagger = new ReHostTagger(this);
    reHostTagger->show();
}
