#include "LttoMainWindow.h"
#include "ui_LttoMainWindow.h"

#include <QMessageBox>
#include <QDebug>
#include <QStandardPaths>
#include <QFileDialog>
#include <QInputDialog>
#include <QAudioDeviceInfo>
#include <QSettings>
#include <QScreen>
#include <QTime>
#include <QDesktopServices>
#include <QFileInfo>
#include <QShortcut>
#include <QThread>

#include "Game.h"
#include "Players.h"
#include "Defines.h"
#include "LttoComms.h"
//#include "Hosting.h"
#include "StyleSheet.h"
#include "FileLoadSave.h"

QTextEdit * LttoMainWindow::textEditLogFile = nullptr;

LttoMainWindow::LttoMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LttoMainWindow),
	playersWindow(nullptr),
	hostGameWindow(nullptr),
	flagsWindow(nullptr),
	aboutForm(nullptr),
	sound_PowerUp(nullptr),
	sound_Powerdown(nullptr)
{
    ui->setupUi(this);

	lttoComms = LttoComms::getInstance();
	textEditLogFile = new QTextEdit;

    setUiMode();

    QCoreApplication::setOrganizationName("Bush And Beyond");
	QCoreApplication::setOrganizationDomain("bushandbeyond.com.au");
    QCoreApplication::setApplicationName("Combobulator");

	loadSettings();

    this->setWindowTitle("Lasertag Combobulator");
    this->setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
	ui->label_BuildNumber	->setText(BUILD_NUMBER);
    gameInfo.setGameType(gameInfo.Ltag0);
	gameInfo.setIsSpiesTeamTagActive(true);
	gameInfo.setNumberOfTeams(0);
	gameInfo.setIsIndoorViewMode(true);
	gameInfo.setCumulativeScoreMode(false);

	timerHeartBeat = new QTimer(this);
	connect(timerHeartBeat,	SIGNAL(timeout() ),	this,	SLOT(heartBeat())	);

	ui->mainToolBar->setVisible(false);
	ui->btn_SpyTeamTags	->setChecked(true);
	ui->btn_SpyTeamTags	->setVisible(false);
	ui->btn_NoTeams		->setChecked(true);
	ui->btn_Spies		->setEnabled(false);
	ui->btn_StartGame	->setEnabled(false);
	ui->btn_Debug		->setVisible(false);
	ui->btn_StartGame	->setVisible(false);
    ui->label_BuildNumber->setVisible(false);
	setLtarControls(false);

	this->setWindowTitle("The Combobulator " + VERSION_NUMBER);

    //QRandomGenerator randomNumber;

    //qsrand(static_cast<uint>(QTime::currentTime().msec()));

    for (int index = 1; index < 25; index++)    playerInfo[index].setPlayerName("Player " + QString::number(index));

    sound_Powerdown = new QSoundEffect(this);
    sound_Powerdown->setSource(QUrl::fromLocalFile(":/resources/audio/shut-down.wav"));

    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::ALT + Qt::Key_X), this), &QShortcut::activated, this, &LttoMainWindow::shutDown);

#ifdef  QT_DEBUG
    gameInfo.setIsThisPlayerInTheGame( 1, true);
    gameInfo.setIsThisPlayerInTheGame(24, true);
    for (int index = 1; index < 25; index++)        gameInfo.setIsThisPlayerInTheGame(index, true);

    ui->btn_StartGame->setEnabled(true);
    ui->btn_Debug->setVisible(true);
#else
    //Without this delay, the QMessagebox prevents the mainWindow from drawing full screen.
    QEventLoop loop;
    QTimer::singleShot(500, &loop, SLOT(quit()));
    loop.exec();
    //processEvents();
    QMessageBox::critical(this,"WARNING","Please disable ALL power saving on this device?\n\nThe device must stay awake at all times during the game, otherwise the Combobulator will not function !");

#endif
}

LttoMainWindow::~LttoMainWindow()
{
	timerHeartBeat->stop();
	lttoComms->closePorts();
	lttoComms->nonBlockingDelay(500);
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

void LttoMainWindow::shutDown()
{
    qDebug() << "\n\n*****\nSHUT_DOWN - LttoMainWindow::shutDown() - Triggered\n*****\n";
    emit on_actionExit_triggered();
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
	for (int index = 0; index <= MAX_PLAYERS; index++)
    {
		 playerInfo[index].setSlowTags(value);
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

	if (!hostGameWindow)	hostGameWindow = new HostGameWindow(this);
	if(hostGameWindow->resetPlayersForNewGame() == false) return;

#ifdef QT_DEBUG
	hostGameWindow->show();
#else
	hostGameWindow->showFullScreen();
#endif
}

void LttoMainWindow::on_btn_SelectPlayers_clicked()
{	
	ui->btn_StartGame->setEnabled(true);

	//remove phantom Team 3 players from 2 team games
//	if(gameInfo.getNumberOfTeams() == 2)
//	{
//		for(int index = 17; index <= 24; index++)
//		{
//			gameInfo.setIsThisPlayerInTheGame(index, false);
//			//qDebug() << "LttoMainWindow::on_btn_StartGame_clicked() - Removing player" << index;
//		}
//	}

	if(!playersWindow)  playersWindow = new PlayersWindow(this);
#ifdef QT_DEBUG
	playersWindow->show();
#else
	playersWindow->showFullScreen();
#endif
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
	qInfo() << "LttoMainWindow::on_btn_SlowTags_clicked() -" << getSlowTags();
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
	qInfo() << "LttoMainWindow::on_btn_MedicMode_clicked() -" << getMedicMode();
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
	qInfo() << "LttoMainWindow::on_btn_TeamTags_clicked() -" << getTeamTags();
}

//--------------------------------------------------------

void LttoMainWindow::on_btn_ReSpawn_clicked()
{
    switch(gameInfo.getIsReSpawnGame())
    {
    case 0:
        #ifndef QT_DEBUG
        QMessageBox::critical(this,"Info","In a Respawn game, the taggers are Neutralised after every 1/10 hit/s\n\nOnce Neutralised the tagger must return to the Combobulator to be respawned, or wait 15 seconds to auto-respawn.\n\nIt is recommended that the Health slider be set to a larger number.\n\n\nN.B. Respawn may not work with all games in LTAR mode, due to limitations in the LTAR taggers.");
        #endif
        ui->btn_ReSpawn->setText("Respawn ON\nAfter 10 hits");
        ui->btn_ReSpawn->setChecked(true);
        gameInfo.setIsReSpawnGame(1);
        for(int index = 0; index <= MAX_PLAYERS; index++)
        {
            playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG,			 true);
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG,	 true);
            playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG,	 true);
        }
        break;

    case 1:
        ui->btn_ReSpawn->setText("Respawn ON\nAfter 1 hit");
        ui->btn_ReSpawn->setChecked(true);
        gameInfo.setIsReSpawnGame(2);
        for(int index = 0; index <= MAX_PLAYERS; index++)
        {
            playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG,			false);
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG,	 true);
            playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG,	 true);
        }
        break;

    case 2:
        ui->btn_ReSpawn->setText("Respawn ON\nNeut for 15s");
        ui->btn_ReSpawn->setChecked(true);
        gameInfo.setIsReSpawnGame(3);
        for(int index = 0; index <= MAX_PLAYERS; index++)
        {
            playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG,			false);
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG,	 true);
            playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG,	false);
        }
        break;

    case 3:
        ui->btn_ReSpawn->setText("Respawn OFF");
        ui->btn_ReSpawn->setChecked(false);
        gameInfo.setIsReSpawnGame(0);
        for(int index = 0; index <= MAX_PLAYERS; index++)
        {
            playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG,			false);
            playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG,	false);
            playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG,	false);
        }
        break;
    }
	qInfo() << "LttoMainWindow::on_btn_ReSpawn_clicked() -" << gameInfo.getIsReSpawnGame();
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
    ui->btn_Flags->setEnabled(false);

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
    ui->btn_Flags->setEnabled(false);

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
    ui->btn_Flags->setEnabled(false);

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
    ui->btn_Flags->setEnabled(false);

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
#ifndef  QT_DEBUG
    QMessageBox::warning(this, "Warning", "Custom Games are not fully supported by the Lasertaggers.\n\nUse this option at your own risk."
										  "\n\nIf no taggers join the game then your settings are invalid and you will need to try again."
										  "\n\nIf you find specific settings that do/don't work please send the details to: combobulator@bushandbeyond.com.au"
										  "\nwith the details so that the software can be improved.");
#endif
	gameInfo.setGameName("CUST");
    ui->btn_NoTeams->setEnabled(true);
    ui->btn_Flags->setEnabled(true);

    switch(gameInfo.getNumberOfTeams())
    {
    //There is actually nothing to do here, as the GameType is always the same, regardless of NumberOfTeams.

    case 1:
    case 2:
    case 3:
    default:
        gameInfo.setGameType(gameInfo.Custom);
        break;
    }
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
    //force Spies to Four, then trigger the button to roll # over to zero.
    gameInfo.setNumberOfSpies(4);
    on_btn_Spies_clicked();
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
	qInfo() << "LttoMainWindow::on_btn_NoTeams_clicked()";
}

void LttoMainWindow::   on_btn_TwoTeams_clicked()              // Find the current gameType and change it to the 2Teams variant.
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
	qInfo() << "LttoMainWindow::on_btn_TwoTeams_clicked()";
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
	qInfo() << "LttoMainWindow::on_btn_ThreeTeams_clicked()";
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
            ui->btn_Spies->setStyleSheet(myStyleSheet.getButtonCheckedCss());
            gameInfo.setNumberOfSpies(index);
        }
        if (gameInfo.getNumberOfSpies() == 4)
        {
            ui->btn_Spies->setText("Spies = 0");
            ui->btn_SpyTeamTags->setVisible(false);
            ui->btn_Spies->setStyleSheet(myStyleSheet.getButtonUnCheckedCss());
            gameInfo.setNumberOfSpies(0);
        }
    }
	qInfo() << "LttoMainWindow::on_btn_Spies_clicked() - Number of Spies =" << gameInfo.getNumberOfSpies();
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

        if(gameInfo.getIsLTARGame())
        {
            ui->actionLTAR_Mode->setChecked(true);
            setLtarControls(true);
        }
        else
        {
            ui->actionLTAR_Mode->setChecked(false);
            setLtarControls(false);
        }

		if(gameInfo.getIsReSpawnGame())
		{
			ui->btn_ReSpawn->setChecked(true);
			ui->btn_ReSpawn->setText("ReSpawn ON");
		}
		else
		{
			ui->btn_ReSpawn->setChecked(false);
			ui->btn_ReSpawn->setText("ReSpawn OFF");
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
	if (playerInfo[0].getSlowTags() == false)    ui->btn_SlowTags	->setText("Slow Tags OFF");
	else                                         ui->btn_SlowTags	->setText("Slow Tags ON");

    ui->btn_TeamTags  ->setChecked(playerInfo[0].getTeamTags() );
	if (playerInfo[0].getTeamTags() == false)    ui->btn_TeamTags	->setText("Team Tags OFF");
	else                                         ui->btn_TeamTags	->setText("Team Tags ON");

    ui->btn_MedicMode ->setChecked(playerInfo[0].getMedicMode() );
	if (playerInfo[0].getMedicMode() == false)   ui->btn_MedicMode	->setText("Medic Mode OFF");
	else                                         ui->btn_MedicMode	->setText("Medic Mode ON");

	ui->btn_ReSpawn	  ->setChecked(gameInfo.getIsReSpawnGame() );
	if (gameInfo.getIsReSpawnGame()  == false)	 ui->btn_ReSpawn	->setText("ReSpawn OFF");
	else                                         ui->btn_ReSpawn	->setText("ReSpawn ON");
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
///
///
///
//////////////////////////////////////////////////////////////////

void LttoMainWindow::saveFile()
{	
	if (!fileLoadSave) fileLoadSave = new FileLoadSave(SAVE_MODE, this);
	connect(fileLoadSave, SIGNAL(fileNameUpdated(QString)),	this, SLOT(updateFileName(QString)));
	fileLoadSave->exec();
	delete(fileLoadSave);

	if (fileLoadSaveName.isEmpty())		// this means the [cancel] button was pressed.
	{
		qDebug() << "LttoMainWindow::saveFile() - ABORTED !";
		return;
	}
	else
	{
		QString fileName = fileLoadSaveName;
		qDebug() << "LttoMainWindow::saveFile() - FileName =" << fileLoadSaveName;

		if (!fileName.endsWith(".lto"))
			fileName += ".lto";

		QFile fileToSave(fileName);
		if (!fileToSave.open(QIODevice::WriteOnly))
		{
			QMessageBox::information(this, tr("Unable to open file"), fileToSave.errorString() );
			return;
		}
		QTextStream gameDataOut (&fileToSave);

		gameInfo.streamToFile(gameDataOut);
		playerInfo[0].streamToFile(gameDataOut);            //index 0 is a dummy, the class saves the entire array.
        gameDataOut << "<><><><><><><><><>" << Qt::endl;
        gameDataOut << "EOF" << Qt::endl;
		fileToSave.close();
	}
}

void LttoMainWindow::loadFile()
{
	if (!fileLoadSave) fileLoadSave = new FileLoadSave(LOAD_MODE, this);
	connect(fileLoadSave, SIGNAL(fileNameUpdated(QString)),	this, SLOT(updateFileName(QString)));
	fileLoadSave->exec();
	qDebug() << "LttoMainWindow::loadFile()";

	delete(fileLoadSave);

	openTheFile();
}

void LttoMainWindow::openTheFile()
{
	if (fileLoadSaveName.isEmpty())		// this means the [cancel] button was pressed.
	{
		return;
	}
	else
	{
		QFile fileToLoad(fileLoadSaveName);
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
		gameInfo.setCurrentGameFileName(fileLoadSaveName);
		fileToLoad.close();
	}
}

void LttoMainWindow::loadSettings()
{
    QSettings settings;
	//TODO: Move this to gameInfo::StreamfromFile
	ui->btn_SpyTeamTags->setChecked(settings.value("SpiesTeamTagMode", true).toBool());

	gameInfo.setPowerSaveMode(settings.value("PowerSaveMode", false).toBool());			//N.B. this is not saved into the Game show file !
    gameInfo.setFontSize(settings.value("FontSize").toInt());
    qDebug() << "\t.....FontSize =" << gameInfo.getFontSize();
	gameInfo.setScoreHeaderFontSize(settings.value("ScoreHeaderFontSize").toInt());
	gameInfo.setScoreTableFontSize(settings.value("ScoreTableFontSize").toInt());
	myStyleSheet.setFontSizes();
	myStyleSheet.updateStyleSheet();

	myStyleSheet.setCurrentCSS(settings.value("ColourScheme", myStyleSheet.CssDark).toInt());
	if(myStyleSheet.getCurrentCSS() == myStyleSheet.CssLight)
	{
		ui->actionOutdoorMode->setChecked(true);
		setStyleSheet(myStyleSheet.getCurrentCSSstring());
	}
	else
	{
		ui->actionOutdoorMode->setChecked(false);
		setStyleSheet(myStyleSheet.getCurrentCSSstring());
	}


	gameInfo.setCurrentGameFileName(settings.value("CurrentGameName").toString());
	qDebug() << "LttoMainWindow::loadSettings() - CurrentGameName =" << gameInfo.getCurrentGameFileName();

	if(gameInfo.getCurrentGameFileName() != "")

	loadLastGame = QMessageBox::question(this,"Load Last Game?", "Do you want to load the game: " + gameInfo.getCurrentGameFileName() + "?", QMessageBox::Yes|QMessageBox::No);
	if (loadLastGame == QMessageBox::Yes)
	{
		fileLoadSaveName = gameInfo.getCurrentGameFileName();
		openTheFile();
	}
	else gameInfo.setCurrentGameFileName("");

#ifdef QT_DEBUG
	show();
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(800, 600)).toSize());
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int posX = (screenGeometry.width()  - this->width())  / 2;
    int posY = (screenGeometry.height() - this->height()) / 2;
    move  (settings.value("pos",  QPoint(posX, posY)).toPoint());
    settings.endGroup();
#else
	showFullScreen();
#endif

}

void LttoMainWindow::saveSettings()
{
    //QSettings settings(settingsFile, QSettings::NativeFormat);
    QSettings settings;
    settings.setValue("SpiesTeamTagMode", gameInfo.getIsSpiesTeamTagActive());
    settings.setValue("ColourScheme", myStyleSheet.getCurrentCSS());
	settings.setValue("CurrentGameName", gameInfo.getCurrentGameFileName());
	settings.setValue("FontSize", gameInfo.getFontSize());
	settings.setValue("ScoreHeaderFontSize", gameInfo.getScoreHeaderFontSize());
	settings.setValue("ScoreTableFontSize", gameInfo.getScoreTableFontSize());
	settings.setValue("PowerSaveMode", gameInfo.getPowerSaveMode());

#ifdef QT_DEBUG
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
#endif
}

void LttoMainWindow::sendLogFile()
{
	QString filePath = QStandardPaths::standardLocations( QStandardPaths::DocumentsLocation ).value(0);
	QDir thisDir;
	thisDir.setPath(filePath);
	if (!thisDir.exists()) thisDir.mkpath(filePath);
	QDir::setCurrent(filePath);

	QString filePathAndFileName;
	filePathAndFileName = filePath + "/Combobulator_Log.txt";

	qDebug() << "*****\n" << "LttoMainWindow::sendLogFile()" << filePathAndFileName << "*****\n";

QFileInfo check_file(filePathAndFileName);
// check if file exists and if yes: Is it really a file and no directory?
if (check_file.exists() && check_file.isFile()) qDebug() <<"LttoMainWindow::sendLogFile() - TRUE !!!!! ";
else											qDebug() <<"LttoMainWindow::sendLogFile() - FALSE ???? ";

	QUrl mailString = "mailto:richie@bushandbeyond.com.au?subject=Combobulator%20Log%20File &attachment="+ filePathAndFileName;
	QDesktopServices::openUrl(mailString);
    qDebug() << "LttoMainWindow::sendLogFile()" << mailString;
}

void LttoMainWindow::setSuperSimpleControls()
{
    qDebug() << "Gert Mode is active";
    //set Gert defaults
    setLTARmode(true);

    //set the LTAR flags to off.
    for(int index = 0; index <= MAX_PLAYERS; index++)
    {
        playerInfo[index].setPackedFlags3(0);
    }

    ui->btn_CustomGame->setChecked(true);
    ui->slider_MegaTags->setValue(0);
    ui->slider_StartAmmo->setValue(10);
    ui->btn_Flags->setEnabled(true);

    //hide some controls
    ui->btn_Kings->hide();
    ui->btn_HideAndSeek->hide();
    ui->btn_OwnTheZone->hide();
    ui->btn_Ltag->hide();
    ui->btn_CustomGame->hide();
    ui->btn_MedicMode->hide();
    ui->btn_SlowTags->hide();
    ui->btn_Spies->hide();
    ui->btn_SpyTeamTags->hide();
    ui->btn_TeamTags->hide();
    ui->btn_ThreeTeams->hide();

    ui->label_MegaTags->hide();
    ui->slider_MegaTags->hide();
    ui->label_SleepTime->hide();
    ui->slider_SleepTime->hide();
    ui->label_StartingAmmo->hide();
    ui->slider_StartAmmo->hide();

    ui->menuFile->hide();
    ui->menuBar->hide();
    ui->menuBar->setVisible(false);

    ui->btn_Settings->setText("Shut\nDown");

    gameInfo.setPointsPerSurvivalMinute(0);
    gameInfo.setPointsPerZoneMinute(0);
    gameInfo.setPointsPerKingHit(0);
    gameInfo.setPointsPerKingHitNegative(0);
    gameInfo.setPointsPerTagLandedNegative(0);

    ui->gridLayout->addWidget(ui->slider_Reloads,    8,3,1,2);
    ui->gridLayout->addWidget(ui->btn_NoTeams,       0,0,3,2);
    ui->gridLayout->addWidget(ui->btn_TwoTeams,      0,3,3,2);
    ui->gridLayout->addWidget(ui->btn_ReSpawn,       4,0,2,2);
    ui->gridLayout->addWidget(ui->btn_Flags,         4,3,2,2);
    ui->gridLayout->addWidget(ui->slider_GameTime,  11,3,1,2);
    ui->gridLayout->addWidget(ui->label_GameTime,   10,3,1,1);
    ui->gridLayout->addWidget(ui->label_Reloads,     7,3,1,1);
    ui->gridLayout->addWidget(ui->btn_SelectPlayers,15,3,4,1);
    ui->gridLayout->addWidget(ui->btn_Settings,     17,0,2,1);

    ui->gridLayout->addWidget(ui->label_BlankSpacer, 7,2,1,1);
}

void LttoMainWindow::on_actionExit_triggered()
{
	ui->btn_CustomGame	->setEnabled(false);
	ui->btn_Debug		->setEnabled(false);
	ui->btn_Flags		->setEnabled(false);
	ui->btn_HideAndSeek	->setEnabled(false);
	ui->btn_Kings		->setEnabled(false);
	ui->btn_Ltag		->setEnabled(false);
	ui->btn_MedicMode	->setEnabled(false);
	ui->btn_NoTeams		->setEnabled(false);
	ui->btn_OwnTheZone	->setEnabled(false);
	ui->btn_ReSpawn		->setEnabled(false);
	ui->btn_SelectPlayers->setEnabled(false);
	ui->btn_Settings	->setEnabled(false);
	ui->btn_SlowTags	->setEnabled(false);
	ui->btn_Spies		->setEnabled(false);
	ui->btn_SpyTeamTags	->setEnabled(false);
	ui->btn_StartGame	->setEnabled(false);
	ui->btn_TeamTags	->setEnabled(false);
	ui->btn_ThreeTeams	->setEnabled(false);
	ui->btn_TwoTeams	->setEnabled(false);
	ui->slider_GameTime	->setEnabled(false);
	ui->slider_Health	->setEnabled(false);
	ui->slider_MegaTags	->setEnabled(false);
	ui->slider_Reloads	->setEnabled(false);
	ui->slider_Shields	->setEnabled(false);
	ui->slider_SleepTime->setEnabled(false);
	ui->slider_StartAmmo->setEnabled(false);

	lttoComms->closePorts();
	saveSettings();

#ifndef QT_DEBUG
    sound_Powerdown->setLoopCount(1);
    sound_Powerdown->play();
	   QEventLoop      loop;
	   QTimer::singleShot(850, &loop, SLOT(quit()));
	   loop.exec();
#endif
	QApplication::closeAllWindows();
	QApplication::quitOnLastWindowClosed();
	lttoComms->nonBlockingDelay(100);
	QApplication::quit();
}

void LttoMainWindow::on_actionSet_CountDown_Time_triggered()
{
//	QInputDialog countDownTimeDialog;
//	countDownTimeDialog.setGeometry(100, 100, 400, 400);
//	countDownTimeDialog.setIntValue(gameInfo.getCountDownTime());
//	countDownTimeDialog.setInputMode(QInputDialog::IntInput);
//	countDownTimeDialog.setIntMinimum(5);
//	countDownTimeDialog.setIntMaximum(30);
//	countDownTimeDialog.setWindowTitle("Countdown Time");
//	//countDownTimeDialog.setWindowIcon(QIcon::Question);
//	countDownTimeDialog.setLabelText("Enter Countdown Time (5-30 sec)");

	bool ok = false;

    //int time = gameInfo.getCountDownTime();

//	ok = countDownTimeDialog.exec();

//	qDebug() << " LttoMainWindow::on_actionSet_CountDown_Time_triggered()" << time;


    int cdTime = QInputDialog::getInt(this, "CountDown Time", "Enter Countdown Time (5-30 sec)", gameInfo.getCountDownTime(), 5, 30, 1, &ok);
    if(ok) gameInfo.setCountDownTime(cdTime);
    qDebug() << " LttoMainWindow::on_actionSet_CountDown_Time_triggered()" << cdTime;
}

void LttoMainWindow::on_actionuse_LazerSwarm_triggered()
{
//    qDebug() << "LttoMainWindow::on_actionuse_LazerSwarm_triggered()";
//    if(ui->actionuse_LazerSwarm->isChecked() ) lttoComms.setUseLazerSwarm(true);
//    else                                       lttoComms.setUseLazerSwarm(false);
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
	if(flagsWindow==nullptr) flagsWindow = new FlagsWindow(0, this);
    flagsWindow->setButtonStates();
#ifdef QT_DEBUG
	flagsWindow->show();
#else
	flagsWindow->showFullScreen();
#endif
	qInfo() << "**** LttoMainWindow::on_btn_Flags_clicked() ****";
}

void LttoMainWindow::on_actionAbout_triggered()
{
    aboutForm = new AboutForm(this);
#ifdef QT_DEBUG
	aboutForm->show();
#else
	aboutForm->showFullScreen();
#endif
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

void LttoMainWindow::on_actionLTAR_Mode_triggered()
{
    bool state = ui->actionLTAR_Mode->isChecked();
	qDebug() << "LttoMainWindow::on_actionLTAR_Mode_triggered() -" << state;
    gameInfo.setIsLTARGame(state);
    setLtarControls(state);
}

void LttoMainWindow::on_actionOutdoorMode_triggered()
{
	if(ui->actionOutdoorMode->isChecked())
    {
        myStyleSheet.setCurrentCSS(myStyleSheet.CssLight);
        setStyleSheet(myStyleSheet.getCurrentCSSstring());
		gameInfo.setIsIndoorViewMode(false);
    }
    else
    {
        myStyleSheet.setCurrentCSS(myStyleSheet.CssDark);
        setStyleSheet(myStyleSheet.getCurrentCSSstring());
		gameInfo.setIsIndoorViewMode(true);
    }
    //Update the Spy button CSS (as it may have been customised if pressed)
    if ( gameInfo.getNumberOfSpies() > 0)   ui->btn_Spies->setStyleSheet(myStyleSheet.getButtonCheckedCss());
    else                                    ui->btn_Spies->setStyleSheet(myStyleSheet.getButtonUnCheckedCss());

    //Refresh the screen
#ifdef QT_DEBUG
	show();
#else
	showFullScreen();
#endif
}

void LttoMainWindow::on_btn_Debug_clicked()
{
    qDebug() << "LttoMainWindow::on_btn_Debug_clicked()";

    scoresWindow = new ScoresWindow(this);

    scoresWindow->showFullScreen();

}

void LttoMainWindow::updateFileName(QString newFileName)
{
	qDebug() << "LttoMainWindow::updateFileName()";
	fileLoadSaveName = newFileName;
}

void LttoMainWindow::on_actionEdit_Scoring_triggered()
{
	setScoreParameters = new SetScoreParameters(this);
#ifdef QT_DEBUG
	setScoreParameters->show();
#else
	setScoreParameters->showFullScreen();
#endif
}

void LttoMainWindow::on_actionUpdate_Firmware_triggered()
{
	if(!otaWindow) otaWindow = new OtaWindow(this);
#ifdef QT_DEBUG
	otaWindow->show();
#else
	otaWindow->showFullScreen();
#endif
}

void LttoMainWindow::on_btn_Settings_clicked()
{
	qDebug() << "Settings button clicked";

    if(gameInfo.getIsSuperSimpleMode())
    {
        on_actionExit_triggered();
        return;
    }

    //if(!settingsWindow)
	settingsWindow = new SettingsWindow(this);
	//else qDebug() << "settingsWindowExists";

	connect(settingsWindow, SIGNAL(adjustScoring()),		this, SLOT(on_actionEdit_Scoring_triggered())			);
	connect(settingsWindow, SIGNAL(saveFile()),				this, SLOT(on_actionSave_triggered())					);
	connect(settingsWindow, SIGNAL(loadFile()),				this, SLOT(on_actionLoad_triggered())					);
	connect(settingsWindow, SIGNAL(showAboutBox()),			this, SLOT(on_actionAbout_triggered())					);
	connect(settingsWindow,	SIGNAL(exitApp()),				this, SLOT(on_actionExit_triggered())					);
	connect(settingsWindow, SIGNAL(setLtarMode(bool)),		this, SLOT(setLTARmode(bool))							);
	connect(settingsWindow, SIGNAL(setOutdoorMode(bool)),	this, SLOT(setOutdoorViewMode(bool))					);
//	connect(settingsWindow, SIGNAL(setFontSize()),			this, SLOT(setFontSize())								);

#ifdef QT_DEBUG
	settingsWindow->show();
#else
	settingsWindow->showFullScreen();
#endif
}

void LttoMainWindow::setOutdoorViewMode(bool state)
{
	if(state == false)
	{
		ui->actionOutdoorMode->setChecked(false);
	}
	else
	{
		ui->actionOutdoorMode->setChecked(true);
	}

	emit on_actionOutdoorMode_triggered();
}

//void LttoMainWindow::setFontSize()
//{

//}

void LttoMainWindow::setLTARmode(bool state)
{
	if(state == false)
	{
		ui->actionLTAR_Mode->setChecked(false);
	}
	else
	{
		ui->actionLTAR_Mode->setChecked(true);
	}

	emit on_actionLTAR_Mode_triggered();
}

void LttoMainWindow::heartBeat()
{
	qDebug() << "LttoMainWindow::HeartBeatSignal()";
	if(lttoComms->getHostingCommsActive() == true)	return;
	lttoComms->sendHeartBeat();
	qDebug() << "LttoMainWindow::HeartBeatSignal() - SENT *********************";
}

void LttoMainWindow::on_actiongert_Mode_triggered()
{
    qDebug() << "Gert Mode is now active";

    setSuperSimpleControls();

    gameInfo.setIsSuperSimpleMode(true);
}

void LttoMainWindow::setUiMode()
{
    QStringList arguments = QApplication::arguments();
    bool isSimpleUI = true;
    QStringList results = arguments.filter("gert", Qt::CaseInsensitive);
    if (results.isEmpty()) isSimpleUI = false;

    if(isSimpleUI)
    {
        qDebug() << "Activating Super Simple UI (Gert) Mode";
        gameInfo.setIsSuperSimpleMode(true);
    }
    else
    {
        qDebug() <<"Activating Normal UI Mode";
        gameInfo.setIsSuperSimpleMode(false);
    }

    if(gameInfo.getIsSuperSimpleMode())  setSuperSimpleControls();
}
