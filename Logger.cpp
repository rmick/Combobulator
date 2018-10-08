#include "Logger.h"
#include "ui_Logger.h"

Logger::Logger(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Logger)
{
	ui->setupUi(this);
}

//Logger::~Logger()
//{
//	delete ui;
//}

Logger *Logger::getInstance()
{
	static Logger *instance = new Logger();
	return instance;
}
