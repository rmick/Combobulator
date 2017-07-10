#include "ReHostTagger.h"
#include "ui_ReHostTagger.h"

#include <QDebug>

ReHostTagger::ReHostTagger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReHostTagger)
{
    ui->setupUi(this);
    signalMapper = new QSignalMapper(this);
    MapPlayerButtons();
    SetActivePlayers();
}

ReHostTagger::~ReHostTagger()
{
    delete ui;
}

void ReHostTagger::MapPlayerButtons()
{
    playerButtons.append(0);    // Fake button for Player 0
    playerButtons.append(ui->btn_Player1);
    playerButtons.append(ui->btn_Player2);
    playerButtons.append(ui->btn_Player3);
    playerButtons.append(ui->btn_Player4);
    playerButtons.append(ui->btn_Player5);
    playerButtons.append(ui->btn_Player6);
    playerButtons.append(ui->btn_Player7);
    playerButtons.append(ui->btn_Player8);
    playerButtons.append(ui->btn_Player9);
    playerButtons.append(ui->btn_Player10);
    playerButtons.append(ui->btn_Player11);
    playerButtons.append(ui->btn_Player12);
    playerButtons.append(ui->btn_Player13);
    playerButtons.append(ui->btn_Player14);
    playerButtons.append(ui->btn_Player15);
    playerButtons.append(ui->btn_Player16);
    playerButtons.append(ui->btn_Player17);
    playerButtons.append(ui->btn_Player18);
    playerButtons.append(ui->btn_Player19);
    playerButtons.append(ui->btn_Player20);
    playerButtons.append(ui->btn_Player21);
    playerButtons.append(ui->btn_Player22);
    playerButtons.append(ui->btn_Player23);
    playerButtons.append(ui->btn_Player24);

    //Connect button array to SignalMapper
    for (int x = 1; x < 25; x++)
    {
        connect (playerButtons[x],  SIGNAL(clicked()), signalMapper, SLOT (map()) );
        signalMapper->setMapping(playerButtons[x] ,  x);
    }
    connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(playerButtonPressed(int))   );
}

void ReHostTagger::SetActivePlayers()
{
    for (int x = 1; x < 25; x++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(x) )  playerButtons[x]->setEnabled(true);
        else                                        playerButtons[x]->setEnabled(false);
        qDebug() << "Player " << x << " = " << gameInfo.getIsThisPlayerInTheGame(x);
    }
}

void ReHostTagger::playerButtonPressed(int playerNumber)
{
    qDebug() << "Rehosting Player " << playerNumber;
    //somehow call > HostGameWindow::hostCurrentPlayer() with the playerNumber variable......
}

void ReHostTagger::on_btn_Close_clicked()
{
    deleteLater();
}
