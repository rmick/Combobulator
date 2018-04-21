#include "FileLoadSave.h"
#include "ui_FileLoadSave.h"

#include <QDebug>
#include <QStandardPaths>
#include <QStringListModel>
#include <QMessageBox>
#include "Defines.h"

FileLoadSave::FileLoadSave(int loadSaveMode, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FileLoadSave)
{
	ui->setupUi(this);

	setWorkingDirectory();
	populateFileList();

	qDebug() << "FileLoadSave::FileLoadSave(int loadSaveMode, QWidget *parent)";
	if(loadSaveMode == SAVE_MODE)
	{
		qDebug() << "FileLoadSave::FileLoadSave(int loadSaveMode, QWidget *parent)  - SAVE MODE";
		saveMode = true;
		ui->btn_LoadSave->setText("Save");
		ui->lineEdit_FileName->setEnabled(true);
		ui->label_TitleBox->setText("Save Game");
	}
	else
	{
		qDebug() << "FileLoadSave::FileLoadSave(int loadSaveMode, QWidget *parent)  - LOAD MODE";
		saveMode = false;
		ui->btn_LoadSave->setText("Load");
		ui->lineEdit_FileName->setEnabled(false);
		ui->label_TitleBox->setText("Load Game");
	}

	connect(ui->treeView_FileTree, SIGNAL(clicked(QModelIndex)), this, SLOT(upDateFileName(QModelIndex)));

	this->setWindowState(Qt::WindowFullScreen);
}

FileLoadSave::~FileLoadSave()
{
	delete ui;
}

void FileLoadSave::setWorkingDirectory()
{
	filePath = QStandardPaths::standardLocations( QStandardPaths::AppDataLocation ).value(0);
	thisDir.setPath(filePath);
	if (!thisDir.exists()) thisDir.mkpath(filePath);
	QDir::setCurrent(filePath);
}

void FileLoadSave::populateFileList()
{
	QFont thisFont;
	thisFont.setPointSize(16);

	fileExtensions << "*.lto";
	fileModel = new QFileSystemModel(this);
	fileModel->setRootPath(QDir::currentPath());
	fileModel->setReadOnly(true);
	fileModel->setNameFilters(fileExtensions);
	fileModel->setNameFilterDisables(false);

	ui->treeView_FileTree->setModel(fileModel);
	ui->treeView_FileTree->setRootIndex(fileModel->index(QDir::currentPath()));
	ui->treeView_FileTree->setColumnHidden(1, true);	//Size
	ui->treeView_FileTree->setColumnHidden(2, true);	//Extension
	ui->treeView_FileTree->setColumnHidden(3, true);	//Date-Time
	ui->treeView_FileTree->setFont(thisFont);
	QRect fileBoxSize = ui->treeView_FileTree->geometry();
	int boxWidth = fileBoxSize.width();
	ui->treeView_FileTree->setColumnWidth(0, boxWidth);
}

void FileLoadSave::upDateFileName(QModelIndex selectedFileIndex)
{
	QString selectedFileName = fileModel->fileName(selectedFileIndex);
	ui->lineEdit_FileName->setText(selectedFileName);
	//emit fileNameUpdated(selectedFileName);
}

void FileLoadSave::saveFile()
{
	QString fileName;
	fileName = ui->lineEdit_FileName->text();

	if(fileName == "")
	{
		QMessageBox::critical(this, "Error", "The filename is blank, please enter a name before pressing [Save]" ,QMessageBox::Ok);
		return;
	}

	if (!fileName.endsWith(".lto"))
		fileName += ".lto";

	//Check Filename
	fileModelIndex = new QModelIndex();
//fileModelIndex->

	//qDebug() << "FileLoadSave::saveFile()" << fileModel->rowCount()
	//Iterate through the list
	//Show dialog if exists

	//else Save show.
	emit fileNameUpdated(fileName);
	qDebug() << "FileLoadSave::saveFile() - Returning to MainWindow" << fileName;
	close();
}

void FileLoadSave::loadFile()
{
	QString fileName;
	fileName = ui->lineEdit_FileName->text();
	emit fileNameUpdated(fileName);
	close();
}

void FileLoadSave::on_btn_Cancel_clicked()
{
	emit fileNameUpdated("");	//sets fileName to empty, so that no action takes place.
	close();
}

void FileLoadSave::on_btn_LoadSave_clicked()
{
	qDebug() << "FileLoadSave::on_btn_LoadSave_clicked()  -";
	if(saveMode)
	{
		saveFile();
		qDebug() << "\tSaving Show";
	}
	else
	{
		loadFile();
		qDebug() << "\tLoading Show";
	}
}
