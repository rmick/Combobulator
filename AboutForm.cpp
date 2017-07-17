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
