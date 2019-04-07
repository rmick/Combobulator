#include "SetScoreParameters.h"
#include "ui_SetScoreParameters.h"

SetScoreParameters::SetScoreParameters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetScoreParameters)
{
    ui->setupUi(this);
    loadCurrentValues();
}

SetScoreParameters::~SetScoreParameters()
{
    delete ui;
}

void SetScoreParameters::on_btn_Close_clicked()
{
    updateScores();
    close();
}

void SetScoreParameters::loadCurrentValues()
{
    ui->spinBox_TagsLandedPoints->setValue(gameInfo.getPointsPerTagLanded() );
    ui->spinBox_TaggedOwnTeamPoints->setValue(gameInfo.getPointsPerTagLandedNegative() );
    ui->spinBox_TagsTakenPoints->setValue(gameInfo.getPointsPerTagTaken() );
    ui->spinBox_SurvivalMinutePoints->setValue(gameInfo.getPointsPerSurvivalMinute() );
    ui->spinBox_ZoneTimePoints->setValue(gameInfo.getPointsPerZoneMinute() );
    ui->spinBox_HitTheKingPoints->setValue(gameInfo.getPointsPerKingHit() );
    ui->spinBox_HitOwnKingPoints->setValue(gameInfo.getPointsPerKingHitNegative() );
}

void SetScoreParameters::loadDefaults()
{
    ui->spinBox_TagsLandedPoints->setValue(POINTS_PER_TAG_LANDED);
    ui->spinBox_TaggedOwnTeamPoints->setValue(POINTS_PER_TAG_LANDED_OWN_TEAM);
    ui->spinBox_TagsTakenPoints->setValue(POINTS_PER_HITS_TAKEN);
    ui->spinBox_SurvivalMinutePoints->setValue(POINTS_PER_SURVIVAL_MINUTE);
    ui->spinBox_ZoneTimePoints->setValue(POINTS_PER_ZONE_MINUTE);
    ui->spinBox_HitTheKingPoints->setValue(POINTS_PER_KING_HIT);
    ui->spinBox_HitOwnKingPoints->setValue(POINTS_PER_OWN_KING_HIT);
}

void SetScoreParameters::updateScores()
{
	gameInfo.setPointsPerTagLanded(ui->spinBox_TagsLandedPoints->value());
	gameInfo.setPointsPerTagLandedNegative(ui->spinBox_TaggedOwnTeamPoints->value());
	gameInfo.setPointsPerTagTaken(ui->spinBox_TagsTakenPoints->value());
	gameInfo.setPointsPerSurvivalMinute(ui->spinBox_SurvivalMinutePoints->value());
	gameInfo.setPointsPerZoneMinute(ui->spinBox_ZoneTimePoints->value());
	gameInfo.setPointsPerKingHit(ui->spinBox_HitTheKingPoints->value());
	gameInfo.setPointsPerKingHitNegative(ui->spinBox_HitOwnKingPoints->value());
}

void SetScoreParameters::on_btn_ResetDefaults_clicked()
{
   loadDefaults();
}
