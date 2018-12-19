#ifndef REARRANGEPLAYERS_H
#define REARRANGEPLAYERS_H

#include <QDialog>
#include <QSignalMapper>
#include <QMouseEvent>
#include <QVector>
#include <QButtonGroup>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "Defines.h"
#include "Game.h"
#include "Players.h"

namespace Ui {
class RearrangePlayers;
}

class RearrangePlayers : public QDialog
{
    Q_OBJECT

public:
    explicit RearrangePlayers(QWidget *parent = 0);
    ~RearrangePlayers();

private slots:
    void    on_btn_Close_clicked();
	void	clearPlayerLists();
    void    populateListWithPlayers();
    void    reAssignPlayerData();
    void    reSizeListWidgetButtonWidth();
	void	assignPlayersToTeams(bool isRandom);
	void	on_btn_Randomise_clicked();
	void	on_btn_SetDefaultTeams_clicked();

signals:
	void    dataUpdated();


private:
    Ui::RearrangePlayers    *ui;
    void    resizeEvent(QResizeEvent *);
    int     numberOfPlayersInEachTeam;

protected:

};

#endif // REARRANGEPLAYERS_H
