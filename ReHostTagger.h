#ifndef REHOSTTAGGER_H
#define REHOSTTAGGER_H

#include <QDialog>
#include <QSignalMapper>
#include <QVector>
#include <QButtonGroup>
#include "Defines.h"
#include "Game.h"
#include "Players.h"
#include "LttoComms.h"

namespace Ui {
class ReHostTagger;
}

class ReHostTagger : public QDialog
{
    Q_OBJECT

public:
    explicit ReHostTagger(QWidget *parent = 0);
    ~ReHostTagger();

private slots:
    void    playerButtonPressed(int playerNumber);
    void    on_btn_Close_clicked();

private:
    Ui::ReHostTagger        *ui;
    QSignalMapper           *signalMapper;
    QVector <QPushButton*>   playerButtons;
	LttoComms				*lttoComms;

    bool    closedWithoutSelectingPlayer;

    void    MapPlayerButtons();
    void    SetActivePlayers();
};

#endif // REHOSTTAGGER_H
