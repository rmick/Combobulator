#include "PlayersWindow.h"
#include "ui_PlayersWindow.h"

#include <QDebug>
#include <QButtonGroup>
#include <QElapsedTimer>
#include <QInputDialog>

#include "LttoMainWindow.h"
#include "Game.h"
#include "Players.h"

PlayersWindow::PlayersWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlayersWindow),
    signalMapper(NULL)
{
    SelectedPlayer = 0;
    ui->setupUi(this);
    signalMapper = new QSignalMapper(this);
    SetUpPlayerButtonMapping();
    LoadPlayersForTeams();
    LoadPlayerSettings(0);      // 0 = Global Player
    SetActivePlayers();
}

PlayersWindow::~PlayersWindow()
{
    delete ui;
}

/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::SetUpPlayerButtonMapping()
{
//    QList<QPushButton*> PlayerButtons = findChildren<QPushButton*>(QRegExp("btn_Player*"));
//// They will be found in any order so need to gather together into a vector.
//    QSignalMapper* signalMapper = new QSignalMapper(this);

//    for (int i=0; i<formButtons.count(); i++)
//    {
//           QPushButton *btn = formButtons.at(i);
//           int id = btn->objectName().mid(QString("btn_Player").length()).toInt() - 1;
//           PlayerButtons[id] = btn;
//           MAP_BTN_TO_ACTION(btn, id);
//    }
//    for (int index=0; index<sizeofArray(lastSwitchStates); index++)
//    {
//           lastSwitchStates[index] = lastLedStates[index] = 0;
//    }
//    connect(signalMapper,  SIGNAL(mapped(int)), this, SIGNAL(btnClicked(int)));
//    connect(this,  SIGNAL(btnClicked(int)), SLOT(buttonPushed(int)));



    PlayerButtons.append(0);    // Fake button for Player 0
    PlayerButtons.append(ui->btn_Player1);
    PlayerButtons.append(ui->btn_Player2);
    PlayerButtons.append(ui->btn_Player3);
    PlayerButtons.append(ui->btn_Player4);
    PlayerButtons.append(ui->btn_Player5);
    PlayerButtons.append(ui->btn_Player6);
    PlayerButtons.append(ui->btn_Player7);
    PlayerButtons.append(ui->btn_Player8);
    PlayerButtons.append(ui->btn_Player9);
    PlayerButtons.append(ui->btn_Player10);
    PlayerButtons.append(ui->btn_Player11);
    PlayerButtons.append(ui->btn_Player12);
    PlayerButtons.append(ui->btn_Player13);
    PlayerButtons.append(ui->btn_Player14);
    PlayerButtons.append(ui->btn_Player15);
    PlayerButtons.append(ui->btn_Player16);
    PlayerButtons.append(ui->btn_Player17);
    PlayerButtons.append(ui->btn_Player18);
    PlayerButtons.append(ui->btn_Player19);
    PlayerButtons.append(ui->btn_Player20);
    PlayerButtons.append(ui->btn_Player21);
    PlayerButtons.append(ui->btn_Player22);
    PlayerButtons.append(ui->btn_Player23);
    PlayerButtons.append(ui->btn_Player24);

    //Connect button array to SignalMapper
    for (int x = 1; x<25; x++)
    {
        connect (PlayerButtons[x],  SIGNAL(clicked()), signalMapper, SLOT (map()) );
        //connect (PlayerButtons[x],  SIGNAL(pressed()), signalMapper, SLOT (map()) );
        //connect (PlayerButtons[x],  SIGNAL(released()), signalMapper, SLOT (map()) );
        signalMapper->setMapping(PlayerButtons[x] ,  x);
        PlayerButtons[x]->installEventFilter(this);
    }
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(playerButtonPressed(int))   );
    qDebug() << "Signal Mapping complete";
}

QElapsedTimer elapsedTime;

bool PlayersWindow::eventFilter(QObject *obj, QEvent *event)
{
    if      (event->type() == QEvent::MouseButtonPress)
    {
        qDebug() << "eventFilter::MousePress" << obj;
        elapsedTime.start();
        return false;
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        if (elapsedTime.elapsed() > 1000)
        {
            qDebug() << "eventFilter::MouseRelease" << obj;
            //renamePlayer();
            //PlayerButtons[SelectedPlayer]->setChecked(false);
            return true;
        }
    }
    //else
    return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::LoadPlayersForTeams()
{
    switch (gameInfo.getNumberOfTeams() )
    {
        case 0:
        case 1:
            for (int x = 17; x<25; x++)
            {
                PlayerButtons[x]->setVisible(true);
            }
            break;
        case 2:
            for (int x = 17; x<25; x++)
            {
                PlayerButtons[x]->setVisible(false);
            }
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::LoadPlayerSettings(int PlayerID)
{
    // Index 0 is the global setting.
    ui->slider_Health               ->setValue  (playerInfo[PlayerID].getHealthTags() );
    ui->slider_Shields              ->setValue  (playerInfo[PlayerID].getShieldTime() );
    ui->slider_MegaTags             ->setValue  (playerInfo[PlayerID].getMegaTags() );
    ui->btn_SelectedPlayerSlowTags  ->setChecked(playerInfo[PlayerID].getSlowTags() );

    if (playerInfo[PlayerID].getSlowTags() == false)    ui->btn_SelectedPlayerSlowTags  ->setText("OFF");
    else                                                ui->btn_SelectedPlayerSlowTags  ->setText("ON");

    ui->btn_SelectedPlayerTeamTags  ->setChecked(playerInfo[PlayerID].getTeamTags() );
    if (playerInfo[PlayerID].getTeamTags() == false)    ui->btn_SelectedPlayerTeamTags  ->setText("OFF");
    else                                                ui->btn_SelectedPlayerTeamTags  ->setText("ON");
    int reLoads;
    if (playerInfo[PlayerID].getReloads() == 0) reLoads = 100;
    else reLoads = playerInfo[PlayerID].getReloads();
    ui->slider_Reloads              ->setValue(reLoads);
    ui->slider_Handicap             ->setValue  (playerInfo[PlayerID].getHandicap() ); // Do this last !!!
}

void PlayersWindow::SetActivePlayers()
{
    for (int x = 1; x < 25; x++)
    {
        if      (gameInfo.getIsThisPlayerInTheGame(x) == true) PlayerButtons[x]->setChecked(true);
        else                                                   PlayerButtons[x]->setChecked(false);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::on_btn_ClosePlayerWindow_clicked()
{
    delete(this);
}

void PlayersWindow::on_btn_SelectNone_clicked()
{
    SetPlayerButtons(false);
    ui->label_Handicap->setEnabled(false);
    ui->slider_Handicap->setEnabled(false);
}

void PlayersWindow::on_btn_SelectAll_clicked()
{

    if (ui->btn_EditMode->isChecked() == true) return;
    else SetPlayerButtons(true);
}

void PlayersWindow::SetPlayerButtons(bool state)
{
    for (int x = 1; x<25; x++)
    {
         PlayerButtons[x]->setChecked(state);
         gameInfo.setIsThisPlayerInTheGame(x, state);
    }
}

void PlayersWindow::setPlayerControls(bool state)
{
    ui->label_Handicap->setEnabled(!state);
    ui->slider_Handicap->setEnabled(!state);
    ui->label_Health->setEnabled(state);
    ui->slider_Health->setEnabled(state);
    ui->label_Health->setEnabled(state);
    ui->label_MegaTags->setEnabled(state);
    ui->slider_MegaTags->setEnabled(state);
    ui->label_Reloads->setEnabled(state);
    ui->slider_Reloads->setEnabled(state);
    ui->label_Shields->setEnabled(state);
    ui->slider_Shields->setEnabled(state);
    ui->label_PlayerSlowTags->setEnabled(state);
    ui->btn_SelectedPlayerSlowTags->setEnabled(state);
    ui->label_PlayerTeamTags->setEnabled(state);
    ui->btn_SelectedPlayerTeamTags->setEnabled(state);
}

void PlayersWindow::setSelectedPlayer(int value)
{
    qDebug() << "setSelectedPlayer()";
    SelectedPlayer = value;
    ui->label_LastSelectedPlayer->setText("Selected Player : " + QString::number(value));
    this->setWindowTitle("Selected Player = " +QString::number(value));
    qDebug() << "Selected Player : " << value << endl;
    if (ui->btn_EditMode->isChecked()) setPlayerControls(true);
    else setPlayerControls(false);

}

int PlayersWindow::getSelectedPlayer() const
{
    return SelectedPlayer;
}


//////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::on_btn_SelectedPlayerSlowTags_clicked()
{
    if(ui->btn_SelectedPlayerSlowTags->isChecked() )
    {
        ui->btn_SelectedPlayerSlowTags->setText("ON");
        playerInfo[SelectedPlayer].setSlowTags(true);
    }
    else
    {
        ui->btn_SelectedPlayerSlowTags->setText("OFF");
        playerInfo[SelectedPlayer].setSlowTags(false);
    }
}

void PlayersWindow::on_btn_SelectedPlayerTeamTags_clicked()
{
    if(ui->btn_SelectedPlayerTeamTags->isChecked() )
    {
        ui->btn_SelectedPlayerTeamTags->setText("ON");
        playerInfo[SelectedPlayer].setTeamTags(true);
    }
    else
    {
        ui->btn_SelectedPlayerTeamTags->setText("OFF");
        playerInfo[SelectedPlayer].setTeamTags(false);
    }
}

//////////////////////////////////////////////////////////////////////////////////
//QElapsedTimer elapsedTime;

void PlayersWindow::mousePressEvent(QMouseEvent *me)
{
    qDebug() << "myMousePressEvent::MouseButtonPress" << me;
    elapsedTime.start();
}

void PlayersWindow::mouseReleaseEvent(QMouseEvent *me)
{
    qDebug() << "myMouseReleaseEvent::MouseButtonRelease" << me;
    if (elapsedTime.elapsed() > 1000)
    {
        qDebug() << "renamePlayer()";
    }
    else
    {
        qDebug() << "setSelectedPlayer(value)";
//        setSelectedPlayer(value);
//        if      (gameInfo.getIsThisPlayerInTheGame(value) == false) gameInfo.setIsThisPlayerInTheGame(value, true);
//        else if (gameInfo.getIsThisPlayerInTheGame(value) == true)  gameInfo.setIsThisPlayerInTheGame(value, false);

//        LoadPlayerSettings(value);
//        AdjustSettingsForHandicap(value);
    }
}


int passNum = 0;
void PlayersWindow::playerButtonPressed(int value)
{

    qDebug() << QEvent::MouseButtonPress << "Pass #:" << ++passNum;

    if (QEvent::MouseButtonPress)
    {
        qDebug() << "playerButtonPressed::MouseButtonPress";
        setSelectedPlayer(value);
        elapsedTime.start();
    }
    if (QEvent::MouseButtonRelease)
    {
        qDebug() << "playerButtonPressed::MouseButtonRelease";
        if (elapsedTime.elapsed() > 1000)
        {
            renamePlayer(value);
        }
        else
        {
            setSelectedPlayer(value);
            if      (gameInfo.getIsThisPlayerInTheGame(value) == false) gameInfo.setIsThisPlayerInTheGame(value, true);
            else if (gameInfo.getIsThisPlayerInTheGame(value) == true)  gameInfo.setIsThisPlayerInTheGame(value, false);

            LoadPlayerSettings(value);
            AdjustSettingsForHandicap(value);
        }
    }
}

void PlayersWindow::renamePlayer(int player)
{
    QString text = QInputDialog::getText(this, tr("Rename Player"), tr("Enter Player name:"), QLineEdit::Normal);

    PlayerButtons[player]->setText(text);
}



/////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::AdjustSettingsForHandicap(int value)
{
    int health      = playerInfo[value].getHealthTags();
    int shields     = playerInfo[value].getShieldTime();
    int megas       = playerInfo[value].getMegaTags();
    int reloads     = playerInfo[value].getReloads();

    if (playerInfo[value].getHandicap() == 0)
    {
        LoadPlayerSettings(value);
    }
    else
    {
        HandicapAdjust("Health",   health);
        HandicapAdjust("Shields",  shields);
        HandicapAdjust("MegaTags", megas);
        HandicapAdjust("Reloads",  reloads);
    }
    ui->label_Health  ->setText("Health : "   + (QString::number(health)) + " tags");
    ui->label_Shields ->setText("Shields : "  + (QString::number(shields)) + " seconds");
    ui->label_MegaTags->setText("MegaTags : " + (QString::number(shields)) );
    if (reloads == 0)  ui->label_Reloads->setText("Reloads : Unlimited");
    else ui->label_Reloads->setText("Reloads : " + (QString::number(reloads)) );
}

void PlayersWindow::HandicapAdjust(const QString type, int &value)
{

    //Formula = value + (value * (static_cast<float>(Handicap)/10) )
    value += value * (static_cast<float>(playerInfo[SelectedPlayer].getHandicap() ) /10);
    if (value > 100) value = 100;
    if (value <0)   value = 0;
    if (type == "Reloads")
    {
        if (value == 100) value = 0;
    }
}

//////////////////////////////////////////////////////////////////////////////////

void PlayersWindow::on_btn_EditMode_clicked()
{
    if (ui->btn_EditMode->isChecked() == true)
    {
        setPlayerControls(true);
        ui->btn_EditMode->setText("Edit Handicap");
        ui->PlayerButtonGroup->setExclusive(true);
//        ui->label_Health->setEnabled(true);
//        ui->label_MegaTags->setEnabled(true);
//        ui->label_Reloads->setEnabled(true);
//        ui->label_Shields->setEnabled(true);
//        ui->slider_Health->setEnabled(true);
//        ui->slider_MegaTags->setEnabled(true);
//        ui->slider_Reloads->setEnabled(true);
//        ui->slider_Shields->setEnabled(true);
//        SetPlayerButtons(false);
//        ui->label_Handicap->setEnabled(false);
//        ui->slider_Handicap->setEnabled(false);

    }
    else if (ui->btn_EditMode->isChecked() == false)
    {
        setPlayerControls(false);
        ui->btn_EditMode->setText("Edit Settings");
        ui->PlayerButtonGroup->setExclusive(false);
//        ui->label_Health->setEnabled(false);
//        ui->label_MegaTags->setEnabled(false);
//        ui->label_Reloads->setEnabled(false);
//        ui->label_Shields->setEnabled(false);
//        ui->slider_Health->setEnabled(false);
//        ui->slider_MegaTags->setEnabled(false);
//        ui->slider_Reloads->setEnabled(false);
//        ui->slider_Shields->setEnabled(false);
//        setPlayerControls(true);
//        SetPlayerButtons(false);
//        ui->label_Handicap->setEnabled(true);
//        ui->slider_Handicap->setEnabled(true);
    }
}

//void PlayersWindow::on_btn_HandicapEnabled_clicked()
//{
//    if (ui->btn_HandicapEnabled->isChecked() == true)
//    {
//        ui->btn_HandicapEnabled->setText("Handicap Mode ON");
//        setPlayerControls(false);
//    }
//    else if (ui->btn_HandicapEnabled->isChecked() == false)
//    {
//        ui->btn_HandicapEnabled->setText("Handicap Mode OFF");
//        setPlayerControls(true);
//    }
//}

void PlayersWindow::on_slider_Handicap_valueChanged(int value)
{
    ui->label_Handicap->setText("Handicap : " + QString::number(value) );
    playerInfo[SelectedPlayer].setHandicap(value);
    AdjustSettingsForHandicap(SelectedPlayer);
}

void PlayersWindow::on_slider_Reloads_valueChanged(int value)
{
    if  (value == 100) ui->label_Reloads->setText("Reloads : Unlimited");
    else ui->label_Reloads->setText("Reloads : " + QString::number(value) );
    playerInfo[SelectedPlayer].setReloads(value);
}

void PlayersWindow::on_slider_Health_valueChanged(int value)
{
     ui->label_Health->setText("Health : " + QString::number(value) + " tags");
     playerInfo[SelectedPlayer].setHealthTags(value);
}

void PlayersWindow::on_slider_Shields_valueChanged(int value)
{
     ui->label_Shields->setText("Shields : " + QString::number(value) + " seconds");
     playerInfo[SelectedPlayer].setShieldTime(value);
}

void PlayersWindow::on_slider_MegaTags_valueChanged(int value)
{
     ui->label_MegaTags->setText("MegaTags : " + QString::number(value) );
     playerInfo[SelectedPlayer].setMegaTags(value);
}

void PlayersWindow::on_btn_DeBug_clicked()
{
    for (int x=0; x < 25; x++)
    {
        qDebug()    << "Player ("    << playerInfo[x].getPlayerName() << ") " << x
                    << " in Game="   << gameInfo.getIsThisPlayerInTheGame(x)
                    << " - Handicap:"<< playerInfo[x].getHandicap()
                    << " - Health:"  << playerInfo[x].getHealthTags()
                    << " - Reloads:" << playerInfo[x].getReloads()
                    << " - Shields:" << playerInfo[x].getShieldTime()
                    << " - Megas:"   << playerInfo[x].getMegaTags()
                    << " - SlwTags:" << playerInfo[x].getSlowTags()
                    << " - TmTags:"  << playerInfo[x].getTeamTags();
    }
}
