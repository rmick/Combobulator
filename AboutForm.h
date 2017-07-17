#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <QDialog>

namespace Ui {
class AboutForm;
}

class AboutForm : public QDialog
{
    Q_OBJECT

public:
    explicit AboutForm(QWidget *parent = 0);
    ~AboutForm();

private slots:
    void on_btn_Ok_clicked();

private:
    Ui::AboutForm *ui;
};

#endif // ABOUTFORM_H
