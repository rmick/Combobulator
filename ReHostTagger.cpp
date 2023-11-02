#include "ReHostTagger.h"
#include "ui_ReHostTagger.h"


#include <QDebug>
#include "StyleSheet.h"

ReHostTagger::ReHostTagger(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReHostTagger)
{
    ui->setupUi(this);
    signalMapper = new QSignalMapper(this);
	lttoComms	 = LttoComms::getInstance();
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
	for (int index = 1; index <= MAX_PLAYERS; index++)
    {
        if (gameInfo.getIsThisPlayerInTheGame(index) )
        {
            QString teamNumText     = "Team " + QString::number(((index-1)/8)+1);
            if(gameInfo.getNumberOfTeams() == 0) teamNumText = "";
            playerButtons[index]->setText(teamNumText + playerInfoTemp[index].getPlayerName());
			playerButtons[index]->setEnabled(true);
            playerButtons[index]->setChecked(true);
            playerButtons[index]->setStyleSheet(myStyleSheet.getButtonSelectedCss());
        }
        else
        {
         playerButtons[index]->setEnabled(false);
         playerButtons[index]->setChecked(false);
         playerButtons[index]->setStyleSheet(myStyleSheet.getButtonUnSelectedCss());
        }

		if(gameInfo.getNumberOfTeams() == 2)
		{
			qDebug() <<"ReHostTagger::SetActivePlayers() - 2 teams selected - Divisor" << MAX_PLAYERS*(2/3);
			if(index > 16)
			{
					qDebug() << "ReHostTagger::SetActivePlayers() - index =" << index;
					playerButtons[index]->setVisible(false);
			}
		}
    }


}

void ReHostTagger::playerButtonPressed(int playerNumber)
{
    QString teamNumText     = "Team " + QString::number(((playerNumber-1)/8)+1) + ", ";
    if(gameInfo.getNumberOfTeams() == 0) teamNumText = "";

    qDebug() << "Rehosting Player " << playerNumber;
    ui->label->setText("Rehosting Team " + teamNumText + playerInfoTemp[playerNumber].getPlayerName() + " - Please standby.");
    lttoComms->sendLCDtext("Re-Hosting"                                 , 1, false);
    lttoComms->sendLCDtext(teamNumText                                  , 2, false);
    lttoComms->sendLCDtext(playerInfoTemp[playerNumber].getPlayerName() , 3,  true);

    gameInfo.setPlayerToReHost(playerNumber);
    deleteLater();
}

void ReHostTagger::on_btn_Close_clicked()
{
    deleteLater();
}
