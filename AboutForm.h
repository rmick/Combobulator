#ifndef ABOUTFORM_H
#define ABOUTFORM_H

#include <QDialog>

#include "OtaWindow.h"
#include <QPointer>

namespace Ui {
class AboutForm;
}

class AboutForm : public QDialog
{
    Q_OBJECT

public:
	explicit AboutForm(QWidget *parent = nullptr);
    ~AboutForm();

private slots:
    void on_btn_Ok_clicked();

	void on_btn_UpdateFirmware_clicked();

private:
	Ui::AboutForm		*ui;
	QPointer<OtaWindow>	otaWindow;
};

#endif // ABOUTFORM_H
