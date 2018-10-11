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
#include "Game.h"
#include "Players.h"
#include "Defines.h"
#include "LttoComms.h"
#include "Hosting.h"
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

    QCoreApplication::setOrganizationName("Bush And Beyond");
    QCoreApplication::setOrganizationDomain("www.bushandbeyond.com.au");
    QCoreApplication::setApplicationName("Combobulator");
    myStyleSheet.setCurrentCSS(myStyleSheet.CssDark);
    loadSettings();

    this->setWindowTitle("Lasertag Combobulator");
    this->setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
	ui->label_BuildNumber	->setText(BUILD_NUMBER);
    gameInfo.setGameType(gameInfo.Ltag0);
	gameInfo.setIsSpiesTeamTagActive(true);
	gameInfo.setNumberOfTeams(0);
	gameInfo.setIsIndoorViewMode(true);

	timerHeartBeat = new QTimer(this);
	//timerHeartBeat->start(1000);
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

	//serialUSBcomms.setIsUSBinitialised(false);
	//tcpComms.initialiseTCPsignalsAndSlots();

    qsrand(static_cast<uint>(QTime::currentTime().msec()));
    for (int index = 1; index < 25; index++)    playerInfo[index].setPlayerName("Player " + QString::number(index));

    sound_Powerdown = new QSoundEffect(this);
    sound_Powerdown->setSource(QUrl::fromLocalFile(":/resources/audio/shut-down.wav"));

#ifdef  QT_DEBUG
    gameInfo.setIsThisPlayerInTheGame( 1, true);
    gameInfo.setIsThisPlayerInTheGame(24, true);
    ui->btn_StartGame->setEnabled(true);
	ui->btn_Debug->setVisible(true);
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
	if(!playersWindow)  playersWindow = new PlayersWindow(this);
#ifdef QT_DEBUG
	playersWindow->show();
#else
	playersWindow->showFullScreen();
	//playersWindow->show();
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

//--------------------------------------------------------

void LttoMainWindow::on_btn_ReSpawn_clicked()
{
    if      (gameInfo.getIsReSpawnGame() == false)
    {
		ui->btn_ReSpawn->setText("Respawn ON");
        gameInfo.setIsReSpawnGame(true);
        for(int index = 0; index < 25; index++)
        {
			playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG,			true);
			playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG,	true);
			playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG,	true);
        }
    }
    else if (gameInfo.getIsReSpawnGame() == true)
    {
		ui->btn_ReSpawn->setText("Respawn OFF");
        gameInfo.setIsReSpawnGame(false);
        for(int index = 0; index < 25; index++)
        {
			playerInfo[index].setBitFlags1(NEUTRALISE_10_FLAG,			false);
			playerInfo[index].setBitFlags2(NEUTRALISE_15s_TAGGED_FLAG,	false);
			playerInfo[index].setBitFlags2(SUPPLY_ZONES_REVIVE_FLAG,	false);
        }
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
	QMessageBox::warning(this, "Warning", "Custom Games are not fully supported by the Lasertaggers.\n\nUse this option at your own risk."
										  "\n\nIf no taggers join the game then your settings are invalid and you will need to try again."
										  "\n\nIf you find specific settings that do/don't work please send the details to: combobulator@bushandbeyond.com.au"
										  "\nwith the details so that the software can be improved.");

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
    if (playerInfo[0].getSlowTags() == false)    ui->btn_SlowTags  ->setText("Slow Tags OFF");
    else                                         ui->btn_SlowTags  ->setText("Slow Tags ON");

    ui->btn_TeamTags  ->setChecked(playerInfo[0].getTeamTags() );
    if (playerInfo[0].getTeamTags() == false)    ui->btn_TeamTags  ->setText("Team Tags OFF");
    else                                         ui->btn_TeamTags  ->setText("Team Tags ON");

    ui->btn_MedicMode ->setChecked(playerInfo[0].getMedicMode() );
    if (playerInfo[0].getMedicMode() == false)   ui->btn_MedicMode ->setText("Medic Mode OFF");
    else                                         ui->btn_MedicMode ->setText("Medic Mode ON");

//	ui->btn_ReSpawn	  ->setChecked(gameInfo.getIsReSpawnGame() );
//	if (gameInfo.getIsReSpawnGame()  == false)	 ui->btn_ReSpawn ->setText("ReSpawn OFF");
//	else                                         ui->btn_MedicMode ->setText("ReSpawn ON");
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
		gameDataOut << "<><><><><><><><><>" << endl;
		gameDataOut << "EOF" << endl;
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
		fileToLoad.close();
	}
}

void LttoMainWindow::loadSettings()
{
    QSettings settings;
	//ui->actionuse_LazerSwarm->setChecked(settings.value("LazerswarmMode", true).toBool());
	//lttoComms.setUseLazerSwarm(settings.value("LazerswarmMode", true).toBool());
    ui->btn_SpyTeamTags->setChecked(settings.value("SpiesTeamTagMode", true).toBool());

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

#ifdef QT_DEBUG
	show();
#else
	showFullScreen();
#endif
    settings.beginGroup("MainWindow");
    resize(settings.value("size", QSize(800, 600)).toSize());
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int posX = (screenGeometry.width()  - this->width())  / 2;
    int posY = (screenGeometry.height() - this->height()) / 2;
    move  (settings.value("pos",  QPoint(posX, posY)).toPoint());
    settings.endGroup();
}

void LttoMainWindow::saveSettings()
{
    //QSettings settings(settingsFile, QSettings::NativeFormat);
    QSettings settings;
	//settings.setValue("LazerswarmMode",   lttoComms.getUseLazerSwarm());
    settings.setValue("SpiesTeamTagMode", gameInfo.getIsSpiesTeamTagActive());
    settings.setValue("ColourScheme", myStyleSheet.getCurrentCSS());
#ifdef QT_DEBUG
    settings.beginGroup("MainWindow");
    settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.endGroup();
#endif
}

void LttoMainWindow::sendLogFile()
{
	QString filePath = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).value(0);
	QDir thisDir;
	thisDir.setPath(filePath);
	if (!thisDir.exists()) thisDir.mkpath(filePath);
	QDir::setCurrent(filePath);

	QString filePathAndFileName;
	filePathAndFileName = filePath + "/Combobulator_Log.txt";

	filePathAndFileName = "/Users/Richie/Desktop/Makefile";

	qDebug() << "LttoMainWindow::sendLogFile()" << filePathAndFileName;

QFileInfo check_file(filePathAndFileName);
// check if file exists and if yes: Is it really a file and no directory?
if (check_file.exists() && check_file.isFile()) qDebug() <<"LttoMainWindow::sendLogFile() - TRUE !!!!! ";
else											qDebug() <<"LttoMainWindow::sendLogFile() - FALSE ???? ";

	QUrl mailString = "mailto:richie@bushandbeyond.com.au?subject=Combobulator%20Log%20File &attachment="+ filePathAndFileName;
	QDesktopServices::openUrl(mailString);
	qDebug() << "LttoMainWindow::sendLogFile()" << mailString;
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

	int time = gameInfo.getCountDownTime();

//	ok = countDownTimeDialog.exec();

//	qDebug() << " LttoMainWindow::on_actionSet_CountDown_Time_triggered()" << time;


	time = QInputDialog::getInt(this, "CountDown Time", "Enter Countdown Time (5-30 sec)", gameInfo.getCountDownTime(), 5, 30, 1, &ok);
	if(ok) gameInfo.setCountDownTime(time);
	qDebug() << " LttoMainWindow::on_actionSet_CountDown_Time_triggered()" << time;
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

#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>

void LttoMainWindow::on_btn_Debug_clicked()
{
	qDebug() << "LttoMainWindow::on_btn_Debug_clicked()";
	int foundCount;
	QNetworkConfiguration netcfg;
	QStringList WiFisList;
	QList<QNetworkConfiguration> netcfgList;

	QNetworkConfigurationManager ncm;
	netcfgList = ncm.allConfigurations();
	WiFisList.clear();
	for (auto &x : netcfgList)
	{
		qDebug() << "Bearer Type" << x.bearerTypeName();
		if (x.bearerType() == QNetworkConfiguration::BearerWLAN)
		{
			if(x.name() == "")
				WiFisList << "Unknown(Other Network)";
			else
				WiFisList << x.name();

			qDebug() << x.type();
		}
	}
	for(int i=0; i<WiFisList.size(); i++)
	{
//		bool exist = false;
//        QTreeWidgetItem * item = new QTreeWidgetItem();
//        for(int j=0; j<ui->treeWidgetWiFis->topLevelItemCount(); j++)
//        {
//            QTreeWidgetItem *index = ui->treeWidgetWiFis->topLevelItem(j);
//            QString str = index->text(1);
//            if(str == WiFisList[i])
//            {
//                exist = true;
//                break;
//            }
//        }
//        if(!exist)
//        {
//            item->setTextAlignment(0,Qt::AlignVCenter);
//            item->setTextAlignment(1,Qt::AlignHCenter);
//            item->setText(0,QString::number(++foundCount));
//            item->setText(1,WiFisList[i]);
//            ui->treeWidgetWiFis->addTopLevelItem(item);
//        }
	}



	return;




	bool result = false;
	QNetworkConfiguration networkConfig;
	QNetworkConfigurationManager networkManager;
	auto networkConnection = networkManager.allConfigurations();

	for (auto &networkFound : networkConnection)
	{
		if (networkFound.bearerType() == QNetworkConfiguration::BearerWLAN)
		{
			qDebug() << "TCPComms::connectToCombobulatorWiFi() - Network found: " << networkFound.name();
			if (networkFound.name() == "Combobulator")
			networkConfig = networkFound;
			result = true;
		}
	}
	auto session = new QNetworkSession(networkConfig, this);
	session->open();
	qDebug() << "TCPComms::connectToCombobulatorWiFi() - " << result;



	//sendLogFile();

//	scoresWindow = new ScoresWindow(this);
//#ifdef QT_DEBUG
//	scoresWindow->showFullScreen();
//#else
//	scoresWindow->showFullScreen();
//#endif
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
	if(!settingsWindow) settingsWindow = new SettingsWindow(this);

	connect(settingsWindow, SIGNAL(adjustScoring()),		this, SLOT(on_actionEdit_Scoring_triggered())			);
	connect(settingsWindow, SIGNAL(saveFile()),				this, SLOT(on_actionSave_triggered())					);
	connect(settingsWindow, SIGNAL(loadFile()),				this, SLOT(on_actionLoad_triggered())					);
	connect(settingsWindow, SIGNAL(showAboutBox()),			this, SLOT(on_actionAbout_triggered())					);
	connect(settingsWindow,	SIGNAL(exitApp()),				this, SLOT(on_actionExit_triggered())					);
	connect(settingsWindow, SIGNAL(setLtarMode(bool)),		this, SLOT(setLTARmode(bool))							);
	connect(settingsWindow, SIGNAL(setOutdoorMode(bool)),	this, SLOT(setOutdoorViewMode(bool))					);

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
