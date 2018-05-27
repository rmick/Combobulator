#include "AboutForm.h"
#include "ui_AboutForm.h"

AboutForm::AboutForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutForm)
{
    ui->setupUi(this);
}

AboutForm::~AboutForm()
{
    delete ui;
}

void AboutForm::on_btn_Ok_clicked()
{
    close();
}

void AboutForm::on_btn_UpdateFirmware_clicked()
{
	otaWindow = new OtaWindow(this);
#ifdef QT_DEBUG
	otaWindow->show();
#else
	otaWindow->showFullScreen();
#endif
}
