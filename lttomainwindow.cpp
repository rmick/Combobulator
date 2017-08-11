#include "LttoMainWindow.h"
#include "ui_LttoMainWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QInputDialog>
#include <QTime>
#include "Game.h"
#include "Players.h"
#include "Defines.h"
#include "LttoComms.h"


LttoMainWindow::LttoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LttoMainWindow),
    playersWindow(NULL),
    hostGameWindow(NULL),
    flagsWindow(NULL)

{
    ui->setupUi(this);
    this->setWindowTitle("LTTO Combobulator");
    gameInfo.setGameType(gameInfo.Ltag0);
    ui->btn_NoTeams->setChecked(true);
    gameInfo.setNumberOfTeams(0);
    ui->btn_Spies->setEnabled(false);
    ui->btn_StartGame->setEnabled(false);
    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    for (int index = 1; index < 25; index++)    playerInfo[index].setPlayerName("Player " + QString::number(index));

    serialUSBcomms.initialiseUSBsignalsAndSlots();
    //ui->btn_CustomGame->setVisible(false);


    //TODO: Get rid of this, it is just debug until I store/recall this setting
    ui->actionuse_LazerSwarm->setChecked(true);
    serialUSBactive = true;
    tcpCommsActive = true;
    gameInfo.setIsSpiesTeamTagActive(false);
    //save position on screen to a Qsettings file



    //TODO: Remove these, they are for testing only.
    QWidget::move(0,0);
    //End of test/debug code.
}

LttoMainWindow::~LttoMainWindow()
{
    delete ui;
}

//////////////////////////////////////////////////////////////////////////////////

bool LttoMainWindow::getTeamTags() const
{
    return playerInfo[0].getTeamTags();
}

void LttoMainWindow::setTeamTags(bool value)
{
    for (int x=0; x<25;x++)
    {
         playerInfo[x].setTeamTags(value);
    }
}

void LttoMainWindow::SetSpiesButtonState(int NumTeams)
{
    if (NumTeams == 0) ui->btn_Spies->setEnabled(false);
    else               ui->btn_Spies->setEnabled(true);
}

bool LttoMainWindow::getMedicMode() const
{
    return playerInfo[0].getMedicMode();
}

void LttoMainWindow::setMedicMode(bool value)
{
    for (int x=0; x<25;x++)
    {
         playerInfo[x].setMedicMode(value);
    }
}

bool LttoMainWindow::getSlowTags() const
{
    return playerInfo[0].getSlowTags();
}

void LttoMainWindow::setSlowTags(bool value)
{
    for (int x=0; x<25;x++)
    {
         playerInfo[x].setSlowTags(value);
    }
}

//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_StartGame_clicked()
{
    if(gameInfo.getTotalNumberOfPlayersInGame() == 0)
    {
        ui->btn_StartGame->setEnabled(false);
        QMessageBox::warning(this,"Error", "There are no players in the game");
        return;
    }

    if ( ! hostGameWindow)
    {
        hostGameWindow = new HostGameWindow(this);
    }

    if(hostGameWindow->resetPlayersForNewGame() == false) return;
    gameInfo.setGameID(hostGameWindow->GetRandomNumber(1,255));
    hostGameWindow->show();
}

void LttoMainWindow::on_btn_SelectPlayers_clicked()
{
    playersWindow = new PlayersWindow(this);
    playersWindow->show();
    ui->btn_StartGame->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_SlowTags_clicked()
{
    if      (getSlowTags() == false)
    {
        ui->btn_SlowTags->setText("Slow Tags ON");
        setSlowTags(true);
    }
    else if (getSlowTags() == true)
    {
        ui->btn_SlowTags->setText("Slow Tags OFF");
        setSlowTags(false);
    }
}

void LttoMainWindow::on_btn_MedicMode_clicked()
{
    if      (getMedicMode() == false)
    {
        ui->btn_MedicMode->setText("Medic Mode ON");
        setMedicMode(true);
    }
    else if (getMedicMode() == true)
    {
        ui->btn_MedicMode->setText("Medic Mode OFF");
        setMedicMode(false);
    }
}

void LttoMainWindow::on_btn_TeamTags_clicked()
{
    if      (getTeamTags() == false)
    {
        ui->btn_TeamTags->setText("Team Tags ON");
        setTeamTags(true);
    }
    else if (getTeamTags() == true)
    {
        ui->btn_TeamTags->setText("Team Tags OFF");
        setTeamTags(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_Ltag_clicked()
{
    gameInfo.setGameName("LTAG");
    ui->btn_NoTeams->setEnabled(true);
    if      (gameInfo.getNumberOfTeams() == 0)   gameInfo.setGameType(gameInfo.Ltag0);
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.Ltag2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.Ltag3);
}

void LttoMainWindow::on_btn_HideAndSeek_clicked()
{
    gameInfo.setGameName("SEEK");
    ui->btn_NoTeams->setEnabled(false);
    if      (gameInfo.getNumberOfTeams() == 0)
    {
        ui->btn_TwoTeams->setChecked(true);
        gameInfo.setGameType(gameInfo.HideSeek2);
        gameInfo.setNumberOfTeams(2);
    }
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.HideSeek2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.HideSeek3);
}

void LttoMainWindow::on_btn_Kings_clicked()
{
    gameInfo.setGameName("KING");
    ui->btn_NoTeams->setEnabled(false);
    if      (gameInfo.getNumberOfTeams() == 0)  //Zero teams is not valid, force to 2 teams.
    {
        ui->btn_TwoTeams->setChecked(true);
        gameInfo.setGameType(gameInfo.Kings2);
        gameInfo.setNumberOfTeams(2);
    }
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.Kings2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.Kings3);
}

void LttoMainWindow::on_btn_OwnTheZone_clicked()
{
    gameInfo.setGameName("ZONE");
    ui->btn_NoTeams->setEnabled(true);
    if      (gameInfo.getNumberOfTeams() == 0)   gameInfo.setGameType(gameInfo.OwnZone0);
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.OwnZone2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.OwnZone3);
}

void LttoMainWindow::on_btn_CustomGame_clicked()        //TODO: Add all the team options, etc
{
    gameInfo.setGameType(12);
    gameInfo.setGameID(42);
}
//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_NoTeams_clicked()
{
    gameInfo.setNumberOfTeams(0);
    ui->btn_Spies->setEnabled(false);
    if (gameInfo.getGameType() >= gameInfo.Ltag0      && gameInfo.getGameType() <= gameInfo.Ltag3)      gameInfo.setGameType(gameInfo.Ltag0);
    if (gameInfo.getGameType() >= gameInfo.OwnZone0   && gameInfo.getGameType() <= gameInfo.OwnZone3)   gameInfo.setGameType(gameInfo.OwnZone0);
}

void LttoMainWindow::on_btn_TwoTeams_clicked()
{
    gameInfo.setNumberOfTeams(2);
    ui->btn_Spies->setEnabled(true);
    switch(gameInfo.getGameType() )
    {
    case Game::Ltag0:
    case Game::Ltag2:
    case Game::Ltag3:
        gameInfo.setGameType(Game::Ltag2);
        break;
    case Game::HideSeek2:
    case Game::HideSeek3:
        gameInfo.setGameType(Game::HideSeek2);
        break;
    case Game::Kings2:
    case Game::Kings3:
        gameInfo.setGameType(Game::Kings2);
        break;
    case Game::OwnZone0:
    case Game::OwnZone2:
    case Game::OwnZone3:
        gameInfo.setGameType(Game::OwnZone2);
        break;
    }
}

void LttoMainWindow::on_btn_ThreeTeams_clicked()
{
    gameInfo.setNumberOfTeams(3);
    ui->btn_Spies->setEnabled(true);
    switch(gameInfo.getGameType() )
    {
        case Game::Ltag0:
        case Game::Ltag2:
        case Game::Ltag3:
            gameInfo.setGameType(Game::Ltag3);
            break;
        case Game::HideSeek2:
        case Game::HideSeek3:
            gameInfo.setGameType(Game::HideSeek3);
            break;
        case Game::Kings2:
        case Game::Kings3:
            gameInfo.setGameType(Game::Kings3);
            break;
        case Game::OwnZone0:
        case Game::OwnZone2:
        case Game::OwnZone3:
            gameInfo.setGameType(Game::OwnZone3);
            break;
        case Game::Special:
            gameInfo.setGameType(Game::Special);
            break;
    }
}

//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_slider_Health_valueChanged(int value)
{
    ui->label_Health->setText("Health : " + QString::number(value) + " tags");
    for (int x=0; x<25;x++)
    {
         playerInfo[x].setHealthTags(value);
    }
}

void LttoMainWindow::on_slider_Reloads_valueChanged(int value)
{
    if  (value == 100) ui->label_Reloads->setText("Reloads : Unlimited");
    else               ui->label_Reloads->setText("Reloads : " + QString::number(value) );

    for (int x=0; x<25;x++)
    {
        playerInfo[x].setReloads(value);
    }
}

void LttoMainWindow::on_slider_Shields_valueChanged(int value)
{
    ui->label_Shields->setText("Shields : " + QString::number(value) + " seconds");
    for (int x=0; x<25;x++)
    {
         playerInfo[x].setShieldTime(value);
    }
}

void LttoMainWindow::on_slider_MegaTags_valueChanged(int value)
{
    if  (value == 100) ui->label_MegaTags->setText("MegaTags : Unlimited");
    else               ui->label_MegaTags->setText("MegaTags : " + QString::number(value) );

    for (int x=0; x<25;x++)
    {
         playerInfo[x].setMegaTags(value);
    }
}

void LttoMainWindow::on_slider_GameTime_valueChanged(int value)
{
    ui->label_GameTime ->setText("Game Time : " + QString::number(value) + " minutes");
    gameInfo.setGameLength(value);
}


void LttoMainWindow::on_btn_Spies_clicked()
{
    for (int x =0; x<4; x++)
    {
        if (gameInfo.getNumberOfSpies() == x)
        {
            ui->btn_Spies->setText("Spies = "+QString::number(++x) );
            gameInfo.setNumberOfSpies(x);
        }
        if (gameInfo.getNumberOfSpies() == 4)
        {
            ui->btn_Spies->setText("Spies = 0");
            gameInfo.setNumberOfSpies(0);
        }
    }
}

//////////////////////////////////////////////////////////////////

void LttoMainWindow::UpdateGameControlSettings()
{
    switch (gameInfo.getGameType() )
    {
        case Game::Ltag0:
            ui->btn_Ltag->setChecked(true);
            ui->btn_NoTeams->setChecked(true);
            ui->btn_Spies->setEnabled(false);
            gameInfo.setNumberOfSpies(0);
            break;
        case Game::Ltag2:
            ui->btn_Ltag->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::Ltag3:
            ui->btn_Ltag->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::HideSeek2:
            ui->btn_HideAndSeek->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::HideSeek3:
            ui->btn_HideAndSeek->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::Kings2:
            ui->btn_Kings->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::Kings3:
            ui->btn_Kings->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::OwnZone0:
            ui->btn_OwnTheZone->setChecked(true);
            ui->btn_NoTeams->setChecked(true);
            ui->btn_Spies->setEnabled(false);
            gameInfo.setNumberOfSpies(0);
            break;
        case Game::OwnZone2:
            ui->btn_OwnTheZone->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        case Game::OwnZone3:
            ui->btn_OwnTheZone->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            ui->btn_Spies->setEnabled(true);
            break;
        default:
            qDebug() << "No match in the switch for : " << gameInfo.getGameType();
    }

    switch (gameInfo.getNumberOfSpies() )
    {
        case 0:
            ui->btn_Spies->setText("Spies = 0");
            break;
        case 1:
            ui->btn_Spies->setText("Spies = 1");
            break;
        case 2:
            ui->btn_Spies->setText("Spies = 2");
            break;
        case 3:
            ui->btn_Spies->setText("Spies = 3");
            break;
        default:
            qDebug() << "No match for Spy Count !!!!";
    }
    ui->slider_GameTime ->setValue(gameInfo.getGameLength() );
}

//////////////////////////////////////////////////////////////////

void LttoMainWindow::UpdateGlobalPlayerControlSettings()
{
    // Index 0 is the global setting.
    ui->slider_Health               ->setValue  (playerInfo[0].getHealthTags() );
    ui->slider_Shields              ->setValue  (playerInfo[0].getShieldTime() );
    ui->slider_MegaTags             ->setValue  (playerInfo[0].getMegaTags() );

    ui->btn_SlowTags  ->setChecked(playerInfo[0].getSlowTags() );
    if (playerInfo[0].getSlowTags() == false)    ui->btn_SlowTags  ->setText("Slow Tags OFF");
    else                                         ui->btn_SlowTags  ->setText("Slow Tags ON");

    ui->btn_TeamTags  ->setChecked(playerInfo[0].getTeamTags() );
    if (playerInfo[0].getTeamTags() == false)    ui->btn_TeamTags  ->setText("Team Tags OFF");
    else                                         ui->btn_TeamTags  ->setText("Team Tags ON");

    ui->btn_MedicMode ->setChecked(playerInfo[0].getMedicMode() );
    if (playerInfo[0].getMedicMode() == false)   ui->btn_MedicMode ->setText("Medic Mode OFF");
    else                                         ui->btn_MedicMode ->setText("Medic Mode ON");

//    int reLoads;
//    if (playerInfo[0].getReloads() == 0) reLoads = 100;
//    else reLoads = playerInfo[0].getReloads();
//    ui->slider_Reloads              ->setValue(reLoads);
      ui->slider_Reloads->setValue(playerInfo[0].getReloads());
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////

void LttoMainWindow::saveFile()
{
    QString path = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).value(0);

    QDir myDir(path);
    if (!myDir.exists()) myDir.mkpath(path);
    QDir::setCurrent(path);

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Game Configuration"), "", tr("Ltto Game (*.lto);; All Files (*)"));
    if (fileName.isEmpty()) return;
    else
    {
        QFile fileToSave(fileName);
        if (!fileToSave.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"), fileToSave.errorString() );
            return;
        }
        QTextStream gameDataOut (&fileToSave);

        gameInfo.streamToFile(gameDataOut);
        playerInfo[0].streamToFile(gameDataOut);            //index 0 is a dummy, the class saves the entire array.
        gameDataOut << "<><><><><><><><><>" << endl;
        gameDataOut << "EOF" << endl;
    }

}

void LttoMainWindow::loadFile()
{
    QString path = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).value(0);

    QDir myDir(path);
    if (!myDir.exists()) myDir.mkpath(path);
    QDir::setCurrent(path);

    qDebug() << "Loading show file";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Ltto Game"), "", tr("Ltto Game (*.lto);; All Files (*)") );
    if (fileName.isEmpty()) return;
    else
    {
        QFile fileToLoad(fileName);
        if (!fileToLoad.open(QIODevice::ReadOnly))
        {
            QMessageBox::information(this, tr("Unable to open file"), fileToLoad.errorString() );
            return;
        }
        QTextStream gameDataIn (&fileToLoad);

        gameInfo.streamFromFile(gameDataIn);
        playerInfo[0].streamFromFile(gameDataIn);

        UpdateGameControlSettings();
        UpdateGlobalPlayerControlSettings();
        gameInfo.setNumberOfTeams(gameInfo.getNumberOfTeams()); //This updates the Flags2 bits 1+2

        //Check for Players, if there are at least two players, enable the Start button.
        if (gameInfo.getTotalNumberOfPlayersInGame() > 1) ui->btn_StartGame->setEnabled(true);

    }
}

void LttoMainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void LttoMainWindow::on_actionSet_CountDown_Time_triggered()
{
    bool ok = false;
    int time = QInputDialog::getInt(this, "CountDown Time", "Enter Countdown Time (5-30 sec)", gameInfo.getCountDownTime(), 5, 30, 1, &ok);
    if(ok) gameInfo.setCountDownTime(time);
}

void LttoMainWindow::on_actionuse_LazerSwarm_triggered()
{
    if(ui->actionuse_LazerSwarm->isChecked() ) lttoComms.setUseLazerSwarm(true);
    else                                       lttoComms.setUseLazerSwarm(false);
}

void LttoMainWindow::on_actionSave_triggered()
{
    saveFile();
}

void LttoMainWindow::on_actionLoad_triggered()
{
    loadFile();
}

void LttoMainWindow::on_actionUSB_Serial_triggered()
{
    if(ui->actionUSB_Serial->isChecked()) serialUSBactive = true;
    else                                  serialUSBactive = false;
}

void LttoMainWindow::on_btn_Flags_clicked()
{
    if(flagsWindow==NULL) flagsWindow = new FlagsWindow;
    flagsWindow->show();
}

void LttoMainWindow::on_actionAbout_triggered()
{
    //TODO:Open an About form
}
