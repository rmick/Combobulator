#ifndef PLAYERSWINDOW_H
#define PLAYERSWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QSignalMapper>
#include <QVector>
#include <QElapsedTimer>
#include "FlagsWindow.h"

namespace Ui {
 class PlayersWindow;
}

class PlayersWindow : public QDialog
{
    Q_OBJECT

public:
    explicit PlayersWindow(QWidget *parent = 0);
    ~PlayersWindow();

    int getSelectedPlayer() const;
    void setSelectedPlayer(int value);

    void RenamePlayerTeamButtons(int numTeams);

private slots:
    void on_btn_ClosePlayerWindow_clicked();

    void on_btn_SelectAll_clicked();

    void on_btn_SelectNone_clicked();

    void on_btn_SelectedPlayerSlowTags_clicked();

    void on_btn_EditMode_clicked();

    void on_slider_Handicap_valueChanged(int value);

    void on_slider_Reloads_valueChanged(int value);

    void on_slider_Health_valueChanged(int value);

    void on_slider_Shields_valueChanged(int value);

    void on_slider_MegaTags_valueChanged(int value);

    void on_btn_SelectedPlayerTeamTags_clicked();

    void playerButtonPressed(int value);

    void playerButtonReleased(int value);

//    void on_btn_Test_clicked();

    void on_btn_Flags_clicked();

//    void on_slider_SleepTimeOut_valueChanged(int value);

    void on_slider_StartAmmo_valueChanged(int value);

    void on_btn_ChangePlayers_clicked();

private:
    Ui::PlayersWindow   *ui;
    QSignalMapper       *signalMapperClicked;
    QSignalMapper       *signalMapperPressed;
    QSignalMapper       *signalMapperReleased;
    QPalette            *paletteRed;
    QPalette            *paletteGreen;
    QElapsedTimer       elapsedTime;
    FlagsWindow         *flagsWindow;

    QVector <QPushButton*> PlayerButtons;
    int SelectedPlayer;
    void SetUpPlayerButtonMapping();
    void LoadPlayersForTeams();
    void LoadPlayerSettings(int PlayerID);
    void SetActivePlayers();
    void ShowButton();
    void SetPlayerControls(int state, int mode);
    void SetPlayerButtons(bool state);
    void RenamePlayer(int player);
    void AdjustSettingsForHandicap(int currentPlayer);
};

#endif // PLAYERSWINDOW_H














//Alternate button mapping method

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

