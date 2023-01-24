#include "FileLoadSave.h"
#include "ui_FileLoadSave.h"

#include <QDebug>
#include <QStandardPaths>
#include <QStringListModel>
#include <QMessageBox>
#include "Defines.h"
#include "StyleSheet.h"

FileLoadSave::FileLoadSave(int loadSaveMode, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FileLoadSave)
{
	ui->setupUi(this);

	setWorkingDirectory();
	populateFileList();
	QFont fileNameFont;
	fileNameFont.setPointSize(myStyleSheet.getFontSizeForFileNames());
	//ui->label_FileName->setFont(fileNameFont);
	ui->lineEdit_FileName->setFont(fileNameFont);

	if(loadSaveMode == SAVE_MODE)
	{
		saveMode = true;
		ui->btn_LoadSave->setText("Save");
		ui->lineEdit_FileName->setEnabled(true);
		ui->label_TitleBox->setText("Save Game");
	}
	else
	{
		saveMode = false;
		ui->btn_LoadSave->setText("Load");
		ui->lineEdit_FileName->setEnabled(false);
		ui->label_TitleBox->setText("Load Game");
	}

	connect(ui->treeView_FileTree, SIGNAL(clicked(QModelIndex)), this, SLOT(upDateFileName(QModelIndex)));


#ifndef QT_DEBUG
    this->setWindowState(Qt::WindowFullScreen);
#endif
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
    qDebug() << "QDir = " + filePath;
}

void FileLoadSave::populateFileList()
{
	QFont thisFont;
	thisFont.setPointSize(myStyleSheet.getFontSizeForFileNames());

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

	//Check if Filename already exists, and give warning.
	QModelIndex parentIndex = fileModel->index(filePath);
	int numberOfRows = fileModel->rowCount(parentIndex);

	for (int index = 0; index < numberOfRows; index++)
	{
		QModelIndex childIndex = fileModel->index(index, 0, parentIndex);
		QString thisFileName = fileModel->data(childIndex).toString();
		if (fileName == thisFileName)
		{
			int responseAction = QMessageBox::critical(this, "Duplicate", "The filename exists, press [Ok] to overwrite, or [cancel] to change the name" , QMessageBox::Cancel, QMessageBox::Ok);
			if (responseAction == QMessageBox::Cancel)
			{
				fileName = "";	//sets fileName to empty, so that no action takes place.
			}
		}
	}
	//Save show.
	emit fileNameUpdated(fileName);
	delete(this);
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
	delete(this);
}

void FileLoadSave::on_btn_LoadSave_clicked()
{
	if(saveMode)
	{
		saveFile();
	}
	else
	{
		loadFile();
	}
}
