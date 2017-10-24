#ifndef SETSCOREPARAMETERS_H
#define SETSCOREPARAMETERS_H

#include <QDialog>
#include "Defines.h"
#include "Game.h"

namespace Ui {
class SetScoreParameters;
}

class SetScoreParameters : public QDialog
{
    Q_OBJECT

public:
    explicit SetScoreParameters(QWidget *parent = 0);
    ~SetScoreParameters();

private slots:
    void on_btn_Close_clicked();

    void on_btn_ResetDefaults_clicked();

private:
    Ui::SetScoreParameters *ui;

    void loadCurrentValues();
    void loadDefaults();
    void updateScores();
};

#endif // SETSCOREPARAMETERS_H
