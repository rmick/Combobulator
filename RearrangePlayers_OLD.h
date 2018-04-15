//#ifndef REARRANGEPLAYERS_H
//#define REARRANGEPLAYERS_H

//#include <QDialog>
//#include <QSignalMapper>
//#include <QMouseEvent>
//#include <QVector>
//#include <QButtonGroup>
//#include <QTableWidget>
//#include <QTableWidgetItem>
//#include "Defines.h"
//#include "Game.h"
//#include "Players.h"

//namespace Ui {
//class RearrangePlayers;
//}

//class RearrangePlayers : public QDialog
//{
//    Q_OBJECT

//public:
//    explicit RearrangePlayers(QWidget *parent = 0);
//    ~RearrangePlayers();

//private slots:
//    void    playerButtonPressed (int playerNumber);
//    void    playerButtonReleased(int playerNumber);
//    void    on_btn_Close_clicked();
//    void    populateListWidgetWithPlayers();

//private:
//    Ui::RearrangePlayers    *ui;
//    QSignalMapper           *signalMapperClicked;
//    QSignalMapper           *signalMapperPressed;
//    QSignalMapper           *signalMapperReleased;
//    QVector <QPushButton*>   playerButtons;
//    QPoint                   offset;
//    QPushButton             *activeButton;
//    QTableWidgetItem        *playerTaggerName[25];
//    QTableWidgetItem        *Title[4];

//    bool    firstPass;

//    void    mapPlayerButtons();

//protected:
//    void mouseMoveEvent     (QMouseEvent *event) override;
//    void mousePressEvent    (QMouseEvent *event) override;
//    void mouseReleaseEvent  (QMouseEvent *event) override;
//};

//#endif // REARRANGEPLAYERS_H
