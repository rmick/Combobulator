#include "HostGameWindow.h"
#include "ui_HostGameWindow.h"
#include <QDebug>


#include "Game.h"
#include "Players.h"
#include "SerialComms.h"
#include "Defines.h"


HostGameWindow::HostGameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostGameWindow)
{
    ui->setupUi(this);

    timerAnnounce   = new QTimer(this);
    timerCountDown  = new QTimer(this);
    tcpComms        = new TCPComms(this);

    connect(timerAnnounce,  SIGNAL(timeout() ),                     this, SLOT(announceGame()) );
    connect(timerCountDown, SIGNAL(timeout() ),                     this, SLOT(sendCountDown()) );
    connect(&serialComms,   SIGNAL(RequestJoinGame(int,int,int) ),  this, SLOT(AssignPlayer(int,int,int)) );
    connect(&serialComms,   SIGNAL(AckPlayerAssignment(int,int) ),  this, SLOT(AddPlayerToGame(int,int)) );
    connect(&serialComms,   SIGNAL(SerialPortFound(QString) ),      this, SLOT(AddSerialPortToListWidget(QString)) );
    connect(&serialComms,   SIGNAL(TimerBlock(bool)),               this, SLOT(SetAnnounceTimerBlock(bool)) );

    serialComms.setUpSerialPort();

    timerAnnounce->start(2000);

    #ifdef Q_OS_ANDROID
    InsertToListWidget("This is Android");
    #endif

    for (int x = 0; x<25;x++) isThisPlayerHosted[x] = false;
    currentPlayer = 1;
    noMorePlayers = false;
    countDownTimeRemaining = 30;
    ui->btn_Cancel->setText("Cancel");
    ui->btn_Rehost->setEnabled(false);

    announceGame();             // this kicks it over immediately, rather than waiting for the first timeout of the 1.5s timer to trigger
}

HostGameWindow::~HostGameWindow()
{
    timerAnnounce->stop();
    timerCountDown->stop();
    serialComms.closeSerialPort();
    delete ui;
}

void HostGameWindow::hideEvent(QHideEvent *)
{
    timerAnnounce->stop();
    timerCountDown->stop();
    serialComms.closeSerialPort();
}

void HostGameWindow::on_btn_Cancel_clicked()
{
    timerAnnounce->stop();
    timerCountDown->stop();
    serialComms.closeSerialPort();
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
    if (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false)
    {
        if (currentPlayer != 0)
        {
            while (gameInfo.getIsThisPlayerInTheGame(currentPlayer) == false) currentPlayer++;
        }
        ui->label->setText("Prepare to Host Tagger : " + QString::number(currentPlayer));
        if (currentPlayer >24)
        {
            ui->label->setText("All players are hosted, press START ");
            ui->btn_StartGame->setEnabled(true);
            currentPlayer = 0;
        }
    }
    else
    {
        hostCurrentPlayer();
    }
}

void HostGameWindow::hostCurrentPlayer()
{
    //qDebug() << "\nAnnounce Game : Player " << currentPlayer;
    InsertToListWidget("Announce Game : Player " + QString::number(currentPlayer));

    serialComms.sendPacket(PACKET,  gameInfo.getGameTypeTx() );
    serialComms.sendPacket(DATA,    gameInfo.getGameIdTx() );
    serialComms.sendPacket(DATA,    gameInfo.getGameLengthTx() );
    serialComms.sendPacket(DATA,    playerInfo[currentPlayer].getHealthTagsTx() ); //BCD
    serialComms.sendPacket(DATA,    playerInfo[currentPlayer].getReloadsTx() );    //BCD     //TODO: FF = Unlimited
    serialComms.sendPacket(DATA,    playerInfo[currentPlayer].getShieldTimeTx() ); //BCD
    serialComms.sendPacket(DATA,    playerInfo[currentPlayer].getMegaTagsTx() );   //BCD    //TODO: FF = Unlimited
    serialComms.sendPacket(DATA,    playerInfo[currentPlayer].getPackedFlags1Tx() );
    serialComms.sendPacket(DATA,    playerInfo[currentPlayer].getPackedFlags2Tx() );
    if (gameInfo.getGameType() == 0x0C)
    {
        serialComms.sendPacket(DATA, "43");  // C
        serialComms.sendPacket(DATA, "55");  // U
        serialComms.sendPacket(DATA, "53");  // S
        serialComms.sendPacket(DATA, "54");  // T
    }
    serialComms.sendPacket(CHECKSUM);
}

int HostGameWindow::calculatePlayerTeam5bits()
{
    int playerTeam5bits = 0;

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
    //qDebug() << "HostGameWindow::AssignPlayer()" << currentPlayer;
    InsertToListWidget("   AssignPlayer()" + QString::number(currentPlayer));

    qDebug() << "HostGameWindow::AssignPlayer() " << gameInfo.getGameID() << ConvertDecToBCD( Game);
//    if(gameInfo.getGameID() == Game)
//    {
//        qDebug() << "HostGameWindow::AssignPlayer() - GameID & TaggerID matched";



        //TODO: Use Flags to see if they requested a specific Team/Player and action it.



        if(isThisPlayerHosted[currentPlayer] == false)
        {
            playerInfo[currentPlayer].setTaggerID(Tagger);

            serialComms.sendPacket(PACKET,  "01");
            serialComms.sendPacket(DATA,    QString::number(Game,   16).toUpper() );
            serialComms.sendPacket(DATA,    QString::number(Tagger, 16).toUpper() );
            serialComms.sendPacket(DATA,    QString::number(calculatePlayerTeam5bits() , 16).toUpper() );
            serialComms.sendPacket(CHECKSUM);
        }
//    }
}

void HostGameWindow::AddPlayerToGame(int Game, int Tagger)
{
    //TODO:check Game is correct
    //TODO:check Tagger is correct  if(gameInfo.getGameID() == Game && playerInfo[currentPlayer].getTaggerID() == Tagger)

    //qDebug() << "HostGameWindow::AddPlayerToGame()" << currentPlayer;
    InsertToListWidget("   AddPlayerToGame()");
    isThisPlayerHosted[currentPlayer] = true;
    if (currentPlayer != 0) currentPlayer++;
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
    timerAnnounce->stop();
    timerCountDown->start(1100);
    countDownTimeRemaining = gameInfo.getCountDownTime();
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
        //TODO:Change the form to show the DeBrief stuff - use a State Machine?
        return;
    }
    serialComms.sendPacket(PACKET , "0");   //CountDown

    QString _gameID = QString::number(ConvertDecToBCD(gameInfo.getGameID() )).toUpper();
    //serialComms.sendPacket(DATA, _gameID);

    serialComms.sendPacket(DATA, gameInfo.getGameIdTx() );

    QString _countDownTime = QString::number(ConvertDecToBCD(countDownTimeRemaining--));
    serialComms.sendPacket(DATA, _countDownTime);
    //serialComms.sendPacket(DATA, QString::number(countDownTimeRemaining, 16).toUpper() );
    serialComms.sendPacket(DATA, "08");   //TODO:Team1 PlayerCount
    serialComms.sendPacket(DATA, "08");   //TODO:Team2 PlayerCount
    serialComms.sendPacket(DATA, "08");   //TODO:Team3 PlayerCount
    serialComms.sendPacket(CHECKSUM);
    InsertToListWidget("CountDownTime = " + QString::number(countDownTimeRemaining));
    //qDebug() << "CountDownTime = " << countDownTimeRemaining << _countDownTime << ":" << _gameID << gameInfo.getGameIdTx();
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
}

void HostGameWindow::on_btn_ShowTCP_clicked()
{
    tcpComms->show();
}

void HostGameWindow::InsertToListWidget(QString lineText)
{
    ui->listWidget_Status->addItem(lineText);
    if (ui->listWidget_Status->count() > 18) ui->listWidget_Status->takeItem(0);
}

void HostGameWindow::on_btn_TestMessage_clicked()
{
        const int Game = 0;

        ////--------------------------------------------------
        //  Host 2TeamRespawn Roonka Game (captured packets from LTTO host)     - WORKS - Tagger screen shows 2TRS
        QString hostedGameType         = "0C";
        QString hostedGameID           = "42";
        QString hostedGameTime         = "15";
        QString hostedTagsAvailable    = "99";
        QString hostedReloadsAvailable = "FF";
        QString hostedShieldTime       = "60";
        QString hostedMegaTags         = "15";
        QString hostedPackedFlags1     = "A8";
        QString hostedPackedFlags2     = "32";
        QString name1                  = "32";
        QString name2                  = "54";
        QString name3                  = "52";
        QString name4                  = "53";
        QString checkSum               = "00";

        if      (Game == 0) qDebug() << endl << "Sending Roonka Game";

        else if (Game ==1)
        {
            qDebug() << endl << "Sending Patent Game";
            ////--------------------------------------------------
            //  Game info from Patent                                           - WORKS - Tagger screen shows 2ZON
            hostedGameType         = "0C";  //HEX
            hostedGameID           = "2C";  //HEX
            hostedGameTime         = "15";  //DEC
            hostedTagsAvailable    = "50";  //DEC
            hostedReloadsAvailable = "FF";  //DEC
            hostedShieldTime       = "45";  //DEC
            hostedMegaTags         = "12";  //DEC
            hostedPackedFlags1     = "28";  //HEX
            hostedPackedFlags2     = "A2";  //HEX
            name1                  = "32";  //HEX
            name2                  = "5A";  //HEX
            name3                  = "4F";  //HEX
            name4                  = "4E";  //HEX
            checkSum               = "ZZ";
            //checkSum               = "E6";  //HEX
        }

        else if (Game ==2)
        {
            qDebug() << endl << "Sending LazerSwarm Game";
            ////--------------------------------------------------
            //  Game info from Patent                                           - WORKS - Tagger responds with RequestJoinGame
            hostedGameType         = "02";  //HEX
            hostedGameID           = "7C";  //HEX
            hostedGameTime         = "10";  //DEC
            hostedTagsAvailable    = "10";  //DEC
            hostedReloadsAvailable = "FF";  //DEC
            hostedShieldTime       = "15";  //DEC
            hostedMegaTags         = "10";  //DEC
            hostedPackedFlags1     = "20";  //HEX
            hostedPackedFlags2     = "01";  //HEX
            name1                  = "00";  //HEX
            name2                  = "00";  //HEX
            name3                  = "00";  //HEX
            name4                  = "00";  //HEX
            checkSum               = "ZZ";
            //checkSum               = "E3";  //HEX
        }

        else if (Game ==3)
        {
            qDebug() << endl << "Sending LTTO default Game";
            ////--------------------------------------------------
            //  Game info from Patent                                               - WORKS - Tagger responds with RequestJoinGame
            hostedGameType         = "02";  //HEX
            hostedGameID           = "A1";  //HEX
            hostedGameTime         = "10";  //DEC
            hostedTagsAvailable    = "10";  //DEC
            hostedReloadsAvailable = "FF";  //DEC
            hostedShieldTime       = "15";  //DEC
            hostedMegaTags         = "10";  //DEC
            hostedPackedFlags1     = "20";  //HEX
            hostedPackedFlags2     = "01";  //HEX
            name1                  = "00";  //HEX
            name2                  = "00";  //HEX
            name3                  = "00";  //HEX
            name4                  = "00";  //HEX
            checkSum               = "ZZ";
            //checkSum               = "08";  //HEX
        }

        else if (Game ==4)
        {
            qDebug() << endl << "Sending Copy of saved LazerSwarm";
            ////--------------------------------------------------
            //  Game info from Patent                                               - NoGo !
            hostedGameType         = "02";
            hostedGameID           = "3B";
            hostedGameTime         = "10";
            hostedTagsAvailable    = "0A";
            hostedReloadsAvailable = "FF";
            hostedShieldTime       = "0F";  //DEC
            hostedMegaTags         = "0A";  //DEC
            hostedPackedFlags1     = "14";  //HEX
            hostedPackedFlags2     = "01";  //HEX
            name1                  = "00";  //HEX
            name2                  = "00";  //HEX
            name3                  = "00";  //HEX
            name4                  = "00";  //HEX
            checkSum               = "ZZ";
            //checkSum               = "08";  //HEX
        }


        serialComms.sendPacket('P', hostedGameType);
        serialComms.sendPacket('D', hostedGameID);
        serialComms.sendPacket('D', hostedGameTime);
        serialComms.sendPacket('D', hostedTagsAvailable);
        serialComms.sendPacket('D', hostedReloadsAvailable);
        serialComms.sendPacket('D', hostedShieldTime);
        serialComms.sendPacket('D', hostedMegaTags);
        serialComms.sendPacket('D', hostedPackedFlags1);
        serialComms.sendPacket('D', hostedPackedFlags2);
        if (name1 != "00")
        {
            serialComms.sendPacket('D', name1);
            serialComms.sendPacket('D', name2);
            serialComms.sendPacket('D', name3);
            serialComms.sendPacket('D', name4);
        }
        serialComms.sendPacket('C');
}



QString HostGameWindow::displayBinary(int number, int digits)
{
    QString output = QString::number(number, 2);
    int padding = digits - output.length();
    while (padding > 0)
    {
        output.prepend('0');
        padding--;
    }
    return output;
}
