#ifndef REHOSTTAGGER_H
#define REHOSTTAGGER_H

#include <QDialog>
#include <QSignalMapper>
#include <QVector>
#include <QButtonGroup>
#include "Game.h"
#include "Players.h"
//#include "HostGameWindow.h"

namespace Ui {
class ReHostTagger;
}

class ReHostTagger : public QDialog  //, public HostGameWindow
{
    Q_OBJECT

public:
    explicit ReHostTagger(QWidget *parent = 0);
    ~ReHostTagger();

    bool getRehostActive() const;
    void setRehostActive(bool value);

private slots:
    void    playerButtonPressed(int playerNumber);

    void    on_btn_Close_clicked();

private:
    Ui::ReHostTagger        *ui;
    QSignalMapper           *signalMapper;
    QVector <QPushButton*>   playerButtons;

    bool    rehostActive;

    void    MapPlayerButtons();
    void    SetActivePlayers();
};

#endif // REHOSTTAGGER_H