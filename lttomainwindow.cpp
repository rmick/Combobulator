#include "LttoMainWindow.h"
#include "ui_LttoMainWindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QInputDialog>
#include <QAudioDeviceInfo>
#include <QSettings>
#include <QTime>
#include "Game.h"
#include "Players.h"
#include "Defines.h"
#include "LttoComms.h"
#include "Hosting.h"

LttoMainWindow::LttoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LttoMainWindow),
    playersWindow(NULL),
    hostGameWindow(NULL),
    flagsWindow(NULL),
    aboutForm(NULL),
    sound_PowerUp(NULL),
    sound_Powerdown(NULL)
{
    ui->setupUi(this);
    QCoreApplication::setOrganizationName("Bush And Beyond");
    QCoreApplication::setOrganizationDomain("www.bushandbeyond.com.au");
    QCoreApplication::setApplicationName("Combobulator");
    //settingsFile = QApplication::applicationDirPath().left(1) + ":/demosettings.ini";
    loadSettings();

    this->setWindowTitle("Lasertag Combobulator");
    this->setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    ui->label_BuildNumber->setText(BUILD_NUMBER);
    gameInfo.setGameType(gameInfo.Ltag0);
    ui->btn_SpyTeamTags->setChecked(true);
    ui->btn_SpyTeamTags->setVisible(false);
    gameInfo.setIsSpiesTeamTagActive(true);
    ui->btn_NoTeams->setChecked(true);
    gameInfo.setNumberOfTeams(0);
    ui->btn_Spies->setEnabled(false);
    ui->btn_StartGame->setEnabled(false);
    setLtarControls(false);
    serialUSBcomms.setIsUSBinitialised(false);
    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    for (int index = 1; index < 25; index++)    playerInfo[index].setPlayerName("Player " + QString::number(index));
    tcpComms.initialiseTCPsignalsAndSlots();

    //Init all the sound effects.
    sound_PowerUp   = new QSoundEffect(this);
    sound_Powerdown = new QSoundEffect(this);
    sound_PowerUp  ->setSource(QUrl::fromLocalFile(":/resources/audio/stinger-power-on.wav"));
    sound_Powerdown->setSource(QUrl::fromLocalFile(":/resources/audio/shut-down.wav"));
    sound_PowerUp->play();
    sound_PowerUp->setVolume(1.0);
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

//--------------------------------------------------------

void LttoMainWindow::SetSpiesButtonState(int NumTeams)
{
    if (NumTeams == 0)
    {
        ui->btn_Spies->setEnabled(false);
        ui->btn_SpyTeamTags->setEnabled(false);
    }
    else
    {
        ui->btn_Spies->setEnabled(true);
        ui->btn_SpyTeamTags->setEnabled(true);
    }
}

//--------------------------------------------------------

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

//--------------------------------------------------------

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

//--------------------------------------------------------

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
    gameInfo.setGameID(host.getRandomNumber(1,255));
#ifdef Q_OS_ANDROID
    hostGameWindow->showFullScreen();
#else
    hostGameWindow->show();
#endif
}

void LttoMainWindow::on_btn_SelectPlayers_clicked()
{
    playersWindow = new PlayersWindow(this);
#ifdef Q_OS_ANDROID
    playersWindow->showFullScreen();
#else
    playersWindow->show();
#endif
    ui->btn_StartGame->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////
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

//--------------------------------------------------------

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

//--------------------------------------------------------

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

/// //////////////////////////////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////
/// //////////////////////////////////////////////////////////////////////////////////
///
void LttoMainWindow::on_btn_Ltag_clicked()
{
    gameInfo.setGameName("LTAG");
    ui->btn_NoTeams->setEnabled(true);

    switch(gameInfo.getNumberOfTeams())
    {
    case 0:
        gameInfo.setGameType(gameInfo.Ltag0);
        break;
    case 2:
        gameInfo.setGameType(gameInfo.Ltag2);
        break;
    case 3:
        gameInfo.setGameType(gameInfo.Ltag3);
        break;
    }
}

void LttoMainWindow::on_btn_HideAndSeek_clicked()
{
    gameInfo.setGameName("SEEK");
    ui->btn_NoTeams->setEnabled(false);

    switch(gameInfo.getNumberOfTeams())
    {
    case 0:
        gameInfo.setGameType(gameInfo.HideSeek2);               //Zero teams is not valid, force to 2 teams.
        ui->btn_TwoTeams->setChecked(true);
        break;
    case 2:
        gameInfo.setGameType(gameInfo.HideSeek2);
        break;
    case 3:
        gameInfo.setGameType(gameInfo.HideSeek3);
        break;
    }
}

void LttoMainWindow::on_btn_Kings_clicked()
{
    gameInfo.setGameName("KING");
    ui->btn_NoTeams->setEnabled(false);

    switch(gameInfo.getNumberOfTeams())
    {
    case 0:
        gameInfo.setGameType(gameInfo.Kings2);                  //Zero teams is not valid, force to 2 teams.
        ui->btn_TwoTeams->setChecked(true);
        break;
    case 2:
        gameInfo.setGameType(gameInfo.Kings2);
        break;
    case 3:
        gameInfo.setGameType(gameInfo.Kings3);
        break;

    }
}

void LttoMainWindow::on_btn_OwnTheZone_clicked()
{
    gameInfo.setGameName("ZONE");
    ui->btn_NoTeams->setEnabled(true);

    switch(gameInfo.getNumberOfTeams())
    {
    case 0:
        gameInfo.setGameType(gameInfo.OwnZone0);
        break;
    case 2:
        gameInfo.setGameType(gameInfo.OwnZone2);
        break;
    case 3:
        gameInfo.setGameType(gameInfo.OwnZone3);
        break;
    }
}

void LttoMainWindow::on_btn_CustomGame_clicked()
{
    gameInfo.setGameName("CUST");
    ui->btn_NoTeams->setEnabled(true);

    switch(gameInfo.getNumberOfTeams())
    {
    //There is actually nothing to do here, as the GameType is always the same, regardless of NumberOfTeams.
    default:
        gameInfo.setGameType(gameInfo.Custom);
        break;
    }
}

void LttoMainWindow::on_btn_LtarGame_clicked()
{
    bool state = ui->btn_LtarGame->isChecked();
    gameInfo.setIsLTARGame(state);
    setLtarControls(state);

//    if(ui->btn_LtarGame->isChecked()) gameInfo.setIsLTARGame(true);
//    else                              gameInfo.setIsLTARGame(false);

//    if(ui->btn_LtarGame->isChecked())   setLtarControls(true);
//    else                                setLtarControls(false);
}

void LttoMainWindow::setLtarControls(bool state)
{
    ui->slider_SleepTime->setVisible(state);
    ui->label_SleepTime->setVisible(state);
    ui->slider_StartAmmo->setVisible(state);
    ui->label_StartingAmmo->setVisible(state);
}
//////////////////////////////////////////////////////////////////////////////////

void LttoMainWindow::on_btn_NoTeams_clicked()
{
    gameInfo.setNumberOfTeams(0);
    SetSpiesButtonState(false);

    switch(gameInfo.getGameType() )                         // Find the current gameType and change it to the NoTeams variant.
    {
    case Game::Ltag0:
    case Game::Ltag2:
    case Game::Ltag3:
        gameInfo.setGameType(Game::Ltag0);
        break;
    case Game::HideSeek2:
    case Game::HideSeek3:
        qDebug() << "Error. This is impossible";
        break;
    case Game::Kings2:
    case Game::Kings3:
        qDebug() << "Error. This is impossible";
        break;
    case Game::OwnZone0:
    case Game::OwnZone2:
    case Game::OwnZone3:
        gameInfo.setGameType(Game::OwnZone0);
        break;
    case Game::Custom:
        gameInfo.setGameType(Game::Custom);
        break;
    }
}

void LttoMainWindow::on_btn_TwoTeams_clicked()              // Find the current gameType and change it to the 2Teams variant.
{
    gameInfo.setNumberOfTeams(2);
    SetSpiesButtonState(true);

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
    case Game::Custom:
        gameInfo.setGameType(Game::Custom);
        break;
    }
}

void LttoMainWindow::on_btn_ThreeTeams_clicked()            // Find the current gameType and change it to the 2Teams variant.
{
    gameInfo.setNumberOfTeams(3);
    SetSpiesButtonState(true);
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
    case Game::Custom:
        gameInfo.setGameType(Game::Custom);
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
    //TODO: Allow for 990 on LTAR.
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

void LttoMainWindow::on_slider_StartAmmo_valueChanged(int value)
{
    if  (value == 100)  ui->label_StartingAmmo->setText("StartingShots : Unlimited");
    else                ui->label_StartingAmmo->setText("StartingShots : " + QString::number(value) );

    for (int x=0; x<25;x++)
    {
         playerInfo[x].setStartingAmmo(value);
    }
}

void LttoMainWindow::on_slider_SleepTime_valueChanged(int value)
{
    if  (value == 0) ui->label_SleepTime->setText("SleepTimeOut : Disabled");
    else             ui->label_SleepTime->setText("SleepTimeOut : " + QString::number(value) );

    for (int x=0; x<25;x++)
    {
         playerInfo[x].setSleepTimeOut(value);
    }
}

void LttoMainWindow::on_btn_Spies_clicked()
{
    for (int index =0; index<4; index++)
    {
        if (gameInfo.getNumberOfSpies() == index)
        {
            ui->btn_Spies->setText("Spies = "+QString::number(++index) );
            ui->btn_SpyTeamTags->setVisible(true);
            ui->btn_Spies->setStyleSheet(BUTTON_CHECKED);
            gameInfo.setNumberOfSpies(index);
        }
        if (gameInfo.getNumberOfSpies() == 4)
        {
            ui->btn_Spies->setText("Spies = 0");
            ui->btn_SpyTeamTags->setVisible(false);
            ui->btn_Spies->setStyleSheet(BUTTON_UNCHECKED);
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

        if(gameInfo.getIsLTARGame())
        {
            ui->btn_LtarGame->setChecked(true);
            setLtarControls(true);
        }
        else
        {
            ui->btn_LtarGame->setChecked(false);
            setLtarControls(false);
        }
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
    ui->slider_Health       ->setValue(playerInfo[0].getHealthTags()  );
    ui->slider_Shields      ->setValue(playerInfo[0].getShieldTime()  );
    ui->slider_MegaTags     ->setValue(playerInfo[0].getMegaTags()    );
    ui->slider_SleepTime    ->setValue(playerInfo[0].getSleepTimeOut());
    ui->slider_StartAmmo    ->setValue(playerInfo[0].getStartingAmmo());
    ui->slider_Reloads      ->setValue(playerInfo[0].getReloads()     );

    ui->btn_SlowTags  ->setChecked(playerInfo[0].getSlowTags() );
    if (playerInfo[0].getSlowTags() == false)    ui->btn_SlowTags  ->setText("Slow Tags OFF");
    else                                         ui->btn_SlowTags  ->setText("Slow Tags ON");

    ui->btn_TeamTags  ->setChecked(playerInfo[0].getTeamTags() );
    if (playerInfo[0].getTeamTags() == false)    ui->btn_TeamTags  ->setText("Team Tags OFF");
    else                                         ui->btn_TeamTags  ->setText("Team Tags ON");

    ui->btn_MedicMode ->setChecked(playerInfo[0].getMedicMode() );
    if (playerInfo[0].getMedicMode() == false)   ui->btn_MedicMode ->setText("Medic Mode OFF");
    else                                         ui->btn_MedicMode ->setText("Medic Mode ON");
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

void LttoMainWindow::loadSettings()
{
    QSettings settings;
    ui->actionuse_LazerSwarm->setChecked(settings.value("LazerswarmMode", true).toBool());
    lttoComms.setUseLazerSwarm(settings.value("LazerswarmMode", true).toBool());
    ui->btn_SpyTeamTags->setChecked(settings.value("SpiesTeamTagMode", true).toBool());

    qDebug() << "LttoMainWindow::loadSettings()" << lttoComms.getUseLazerSwarm();
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(1024, 800)).toSize());
    move  (settings.value("pos",  QPoint(0, 0)).toPoint()); //TODO: Centre this on the screen !
    settings.endGroup();
}

void LttoMainWindow::saveSettings()
{
    //QSettings settings(settingsFile, QSettings::NativeFormat);
    QSettings settings;
    settings.setValue("LazerswarmMode",   lttoComms.getUseLazerSwarm());
    settings.setValue("SpiesTeamTagMode", gameInfo.getIsSpiesTeamTagActive());
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
}

void LttoMainWindow::on_actionExit_triggered()
{
    tcpComms.DisconnectTCP();
    saveSettings();
    sound_Powerdown->setLoopCount(1);
    sound_Powerdown->play();
    lttoComms.nonBlockingDelay(850);
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
    qDebug() << "LttoMainWindow::on_actionuse_LazerSwarm_triggered()";
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

void LttoMainWindow::on_btn_Flags_clicked()
{
    if(flagsWindow==NULL) flagsWindow = new FlagsWindow(0, this);
    flagsWindow->setButtonStates();
#ifdef Q_OS_ANDROID
    flagsWindow->showFullScreen();
#else
    flagsWindow->show();
#endif
}

void LttoMainWindow::on_actionAbout_triggered()
{
    aboutForm = new AboutForm(this);
    aboutForm->show();
}

void LttoMainWindow::on_btn_SpyTeamTags_clicked()
{
    if      (gameInfo.getIsSpiesTeamTagActive() == false)
    {
        ui->btn_SpyTeamTags->setText("Spy TeamTags ON");
        gameInfo.setIsSpiesTeamTagActive(true);
    }
    else if (gameInfo.getIsSpiesTeamTagActive() == true)
    {
        ui->btn_SpyTeamTags->setText("Spy TeamTags OFF");
        gameInfo.setIsSpiesTeamTagActive(false);
    }
}

//    /*-------------------------------------------*/

//    QSlider
//    {
//        min-height: 35px;
//    background: cyan;
//        border-style: outset;
//        border-width: 2px;
//        border-radius: 10px;
//        border-color: grey;
//    }

//    /*QSlider::groove:horizontal {
//            border-style: outset;
//            border-width: 2px;
//            border-radius: 10px;
//            border-color: grey;
//        }*/
//    QSlider::groove:horizontal {
//                        border-style: outset;
//    border-width: 0px;
//    border-radius: 10px;
//    border-color: grey;
//    }

//    QSlider::handle:horizontal {
//                        background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);
//    border: 1px solid #5c5c5c;
//    width: 18px;
//    margin: -2px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */
//    border-style: outset;
//    border-width: 2px;
//    border-radius: 7px;
//    border-color: grey;
//    }
//    /*QSlider::handle:horizontal {
//            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #b4b4b4, stop:1 #8f8f8f);
//            border: 2px *solid #5c5c5c*;
//            width: 15px;
//            radius: 5px;
//            margin: -18px 0; /* handle is placed by default on the contents rect of the groove. Expand outside the groove */
//    /*    border-style: outset;
//            border-width: 2px;
//            border-radius: 5px;
//            border-color: grey;
//        }*/

//    QSlider::add-page:horizontal {
//                          background: rgb(50,220,200);
//    border-style: outset;
//    border-width: 0px;
//    border-radius: 10px;
//    }

//    QSlider::sub-page:horizontal {
//                          background: cyan;
//    border-style: outset;
//    border-width: 0px;
//    border-radius: 10px;
//    }

//    QSlider::sub-page:horizontal:disabled {
//                          background: #bbb;
//    }

//    QSlider::add-page:horizontal:disabled {
//                          background: #eee;
//    }

//    QSlider::handle:horizontal:disabled {
//                        background: #eee;
//    }

//    /*-------------------------------------------*/

