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
    void    populateListWithPlayers();
    void    reAssignPlayerData();
    void    reSizeListWidgetButtonWidth();

private:
    Ui::RearrangePlayers    *ui;
    void    resizeEvent(QResizeEvent *);

    int     numberOfPlayersInEachTeam;

signals:
    void    dataUpdated();

protected:

};

#endif // REARRANGEPLAYERS_H
