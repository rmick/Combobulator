#ifndef FLAGSWINDOW_H
#define FLAGSWINDOW_H

#include <QDialog>
#include "Defines.h"
#include "Players.h"

namespace Ui {
class FlagsWindow;
}

class FlagsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FlagsWindow(QWidget *parent = 0);
    ~FlagsWindow();

private slots:
    void on_btn_Neutralise_clicked();

    void on_btn_ContestedZones_clicked();

    void on_btn_ZonesWithTeams_clicked();

    void on_btn_NeutraliseWhenTagged_clicked();

    void on_btn_SupplyZonesRefillTags_clicked();

    void on_btn_HostileZones_clicked();

    void on_btn_ReverseHuntDirection_clicked();

    void on_btn_Close_clicked();

private:
    Ui::FlagsWindow *ui;
    void updatePlayerFlags1(int bit, bool state, int firstPlayer = 0, int lastPlayer = 25);
    void updatePlayerFlags2(int bit, bool state, int firstPlayer = 0, int lastPlayer = 25);
};

#endif // FLAGSWINDOW_H
