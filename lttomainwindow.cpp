#include "lttomainwindow.h"
#include "ui_lttomainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include "Game.h"
#include "Players.h"


LttoMainWindow::LttoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LttoMainWindow),
    playersWindow(NULL),
    bluetoothWindow(NULL),
    hostGameWindow(NULL),
    portConnect(NULL)

{
    ui->setupUi(this);
    gameInfo.setGameType(gameInfo.Ltag0);
    ui->btn_NoTeams->setChecked(true);
    gameInfo.setNumberOfTeams(0);

    QWidget::move(0,0);
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
    for (int x=1; x<25;x++)
    {
         playerInfo[x].setSlowTags(value);
    }
}

//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_StartGame_clicked()
{
    hostGameWindow = new HostGameWindow(this);
    hostGameWindow->resetPlayersForNewGame();
    hostGameWindow->show();
}

void LttoMainWindow::on_btn_SelectPlayers_clicked()
{
    playersWindow = new PlayersWindow(this);
    playersWindow->show();
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
    qDebug() << "LaserTag: " << gameInfo.getGameType();
}

void LttoMainWindow::on_btn_HideAndSeek_clicked()
{
    gameInfo.setGameName("SEEK");
    ui->btn_NoTeams->setEnabled(false);
    if      (gameInfo.getNumberOfTeams() == 0)
    {
        ui->btn_TwoTeams->setChecked(true);
        gameInfo.setGameType(gameInfo.HideSeek2);
    }
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.HideSeek2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.HideSeek3);
    qDebug() << "HideAndSeek: " << gameInfo.getGameType();
}

void LttoMainWindow::on_btn_Kings_clicked()
{
    gameInfo.setGameName("KING");
    ui->btn_NoTeams->setEnabled(false);
    if      (gameInfo.getNumberOfTeams() == 0)  //Zero teams is not valid, force to 2 teams.
    {
        ui->btn_TwoTeams->setChecked(true);
        gameInfo.setGameType(gameInfo.Kings2);
    }
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.Kings2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.Kings3);
    qDebug() << "Kings: " << gameInfo.getGameType();
}

void LttoMainWindow::on_btn_OwnTheZone_clicked()
{
    gameInfo.setGameName("ZONE");
    ui->btn_NoTeams->setEnabled(true);
    if      (gameInfo.getNumberOfTeams() == 0)   gameInfo.setGameType(gameInfo.OwnZone0);
    else if (gameInfo.getNumberOfTeams() == 2)   gameInfo.setGameType(gameInfo.OwnZone2);
    else if (gameInfo.getNumberOfTeams() == 3)   gameInfo.setGameType(gameInfo.OwnZone3);
    qDebug() << "OwnTheZone: " << gameInfo.getGameType();
}

//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_NoTeams_clicked()
{
    gameInfo.setNumberOfTeams(0);
    qDebug() << "Current Game is: " << gameInfo.getGameType();
    if (gameInfo.getGameType() >= gameInfo.Ltag0      && gameInfo.getGameType() <= gameInfo.Ltag3)      gameInfo.setGameType(gameInfo.Ltag0);
    if (gameInfo.getGameType() >= gameInfo.OwnZone0   && gameInfo.getGameType() <= gameInfo.OwnZone3)   gameInfo.setGameType(gameInfo.OwnZone0);
    qDebug() << "New Game is:" << gameInfo.getGameType();
}

void LttoMainWindow::on_btn_TwoTeams_clicked()
{
    gameInfo.setNumberOfTeams(2);
    qDebug() << "Current Game is: " << gameInfo.getGameType();
    if (gameInfo.getGameType() >= gameInfo.Ltag0      && gameInfo.getGameType() <= gameInfo.Ltag3)      gameInfo.setGameType(gameInfo.Ltag2);
    if (gameInfo.getGameType() >= gameInfo.HideSeek2  && gameInfo.getGameType() <= gameInfo.HideSeek3)  gameInfo.setGameType(gameInfo.HideSeek2);
    if (gameInfo.getGameType() >= gameInfo.Kings2     && gameInfo.getGameType() <= gameInfo.Kings3)     gameInfo.setGameType(gameInfo.Kings2);
    if (gameInfo.getGameType() >= gameInfo.OwnZone0   && gameInfo.getGameType() <= gameInfo.OwnZone3)   gameInfo.setGameType(gameInfo.OwnZone2);
    qDebug() << "New Game is:" << gameInfo.getGameType();
}

void LttoMainWindow::on_btn_ThreeTeams_clicked()
{
    gameInfo.setNumberOfTeams(3);
    qDebug() << "Current Game is: " << gameInfo.getGameType();
    switch(gameInfo.getGameType() )
    {
        case Game::Ltag0:
        case Game::Ltag2:
        case Game::Ltag3:
            gameInfo.setGameType(gameInfo.Ltag3);
            break;
        case Game::HideSeek2:
        case Game::HideSeek3:
            gameInfo.setGameType(gameInfo.HideSeek3);
            break;
        case Game::Kings2:
        case Game::Kings3:
            gameInfo.setGameType(gameInfo.Kings3);
            break;
        case Game::OwnZone0:
        case Game::OwnZone2:
        case Game::OwnZone3:
            gameInfo.setGameType(gameInfo.OwnZone3);
            break;
        case Game::Special:
            gameInfo.setGameType(gameInfo.Special);
            break;
    }
//    if (gameInfo.getGameType() >= gameInfo.Ltag0      && gameInfo.getGameType() <= gameInfo.Ltag3)      gameInfo.setGameType(gameInfo.Ltag3);
//    if (gameInfo.getGameType() >= gameInfo.HideSeek2  && gameInfo.getGameType() <= gameInfo.HideSeek3)  gameInfo.setGameType(gameInfo.HideSeek3);
//    if (gameInfo.getGameType() >= gameInfo.Kings2     && gameInfo.getGameType() <= gameInfo.Kings3)     gameInfo.setGameType(gameInfo.Kings3);
//    if (gameInfo.getGameType() >= gameInfo.OwnZone0   && gameInfo.getGameType() <= gameInfo.OwnZone3)   gameInfo.setGameType(gameInfo.OwnZone3);
     qDebug() << "New Game is:" << gameInfo.getGameType();
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
    else ui->label_Reloads->setText("Reloads : " + QString::number(value) );

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
    ui->label_MegaTags->setText("MegaTags : " + QString::number(value) );
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

void LttoMainWindow::on_btn_Quit_clicked()
{
    QApplication::quit();
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

//////////////////////////////////////////////////////////////////                      // Here we are !!!!!!

void LttoMainWindow::UpdateGameControlSettings()
{
    qDebug() << "Updating Game Type to : " << gameInfo.getGameType();
    switch (gameInfo.getGameType() )
    {
        case Game::Ltag0:
            ui->btn_Ltag->setChecked(true);
            ui->btn_NoTeams->setChecked(true);
            break;
        case Game::Ltag2:
            ui->btn_Ltag->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            break;
        case Game::Ltag3:
            ui->btn_Ltag->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            break;
        case Game::HideSeek2:
            ui->btn_HideAndSeek->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            break;
        case Game::HideSeek3:
            ui->btn_HideAndSeek->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            break;
        case Game::Kings2:
            ui->btn_Kings->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            break;
        case Game::Kings3:
            ui->btn_Kings->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            break;
        case Game::OwnZone0:
            ui->btn_OwnTheZone->setChecked(true);
            ui->btn_NoTeams->setChecked(true);
            break;
        case Game::OwnZone2:
            ui->btn_OwnTheZone->setChecked(true);
            ui->btn_TwoTeams->setChecked(true);
            break;
        case Game::OwnZone3:
            ui->btn_OwnTheZone->setChecked(true);
            ui->btn_ThreeTeams->setChecked(true);
            break;
        default:
            qDebug() << "No match in the switch for : " << gameInfo.getGameType();
    }

    qDebug() << "Setting Spy Count to " << gameInfo.getNumberOfSpies();
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
    qDebug() << "Commence Loading PlayerSettings";
    ui->slider_Health               ->setValue  (playerInfo[0].getHealthTags() );
    ui->slider_Shields              ->setValue  (playerInfo[0].getShieldTime() );
    ui->slider_MegaTags             ->setValue  (playerInfo[0].getMegaTags() );
    qDebug() << "Commence Loading tags";
    ui->btn_SlowTags  ->setChecked(playerInfo[0].getSlowTags() );
    if (playerInfo[0].getSlowTags() == false)    ui->btn_SlowTags  ->setText("Slow Tags OFF");
    else                                         ui->btn_SlowTags  ->setText("Slow Tags ON");
    ui->btn_TeamTags  ->setChecked(playerInfo[0].getTeamTags() );
    if (playerInfo[0].getTeamTags() == false)    ui->btn_TeamTags  ->setText("Team Tags OFF");
    else                                         ui->btn_TeamTags  ->setText("Team Tags ON");
    ui->btn_MedicMode ->setChecked(playerInfo[0].getMedicMode() );
    if (playerInfo[0].getMedicMode() == false)   ui->btn_MedicMode ->setText("Medic Mode OFF");
    else                                         ui->btn_MedicMode ->setText("Medic Mode ON");
qDebug() << "Commence Loading ReLoads";
    int reLoads;
    if (playerInfo[0].getReloads() == 0) reLoads = 100;
    else reLoads = playerInfo[0].getReloads();
    ui->slider_Reloads              ->setValue(reLoads);
qDebug() << "All Player Settings Locked and Loaded :-)";
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////
void LttoMainWindow::on_btnSave_clicked()
{
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

void LttoMainWindow::on_btnLoad_clicked()
{
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
    }
}



void LttoMainWindow::on_btn_Bluetooth_clicked()
{
    bluetoothWindow->show();
}

void LttoMainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void LttoMainWindow::on_btn_CustomGame_clicked()
{
    gameInfo.setGameType(12);
    gameInfo.setGameID(44);
    playerInfo[0].setPackedFlags1(28);
    playerInfo[0].setPackedFlags2(162);
}

void LttoMainWindow::on_actionPorts_triggered()
{
    portConnect = new PortConnect(this);
    portConnect->show();
}

void LttoMainWindow::on_actionSet_CountDown_Time_triggered()
{
    bool ok = false;
    //QInputDialog::IntInput;
    int time = QInputDialog::getInt(this, "CountDown Time", "Enter Countdown Time (5-30 sec)", gameInfo.getCountDownTime(), 5, 30, 1, &ok);
    if(ok) gameInfo.setCountDownTime(time);
}
