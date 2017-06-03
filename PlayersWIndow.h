#ifndef PLAYERSWINDOW_H
#define PLAYERSWINDOW_H

#include <QDialog>
#include <QTimer>
#include <QSignalMapper>
#include <QVector>
#include <QElapsedTimer>

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
    QVector <QPushButton*> PlayerButtons;

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

    void on_btn_DeBug_clicked();

private:
    Ui::PlayersWindow   *ui;
    QSignalMapper       *signalMapper;

    int SelectedPlayer;
    void SetUpPlayerButtonMapping();
    void LoadPlayersForTeams();
    void LoadPlayerSettings(int PlayerID);
    void SetActivePlayers();
    void ShowButton();
    void setPlayerControls(bool state);
    void SetPlayerButtons(bool state);
    void renamePlayer(int player);
    void AdjustSettingsForHandicap(int value);
    void HandicapAdjust(const QString type, int &value);

    bool eventFilter(QObject *obj, QEvent *event);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
};

#endif // PLAYERSWINDOW_H
