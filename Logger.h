#ifndef LOGGER_H
#define LOGGER_H

#include <QWidget>

namespace Ui {
class Logger;
}

class Logger : public QWidget
{
	Q_OBJECT

private:
	Ui::Logger	*ui;
	explicit	Logger(QWidget *parent = nullptr);
	static		Logger *instance;

public:
	static		Logger *getInstance();


};

#endif // LOGGER_H
