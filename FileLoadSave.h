#ifndef FILELOADSAVE_H
#define FILELOADSAVE_H

#include <QDialog>
#include <QDir>
#include <QFileSystemModel>
#include <QModelIndex>
#include <QTreeView>

namespace Ui {
class FileLoadSave;
}

class FileLoadSave : public QDialog
{
	Q_OBJECT

public:
	explicit FileLoadSave(int loadSaveMode, QWidget *parent = 0);
	~FileLoadSave();

private slots:
	void	on_btn_Cancel_clicked();

	void	on_btn_LoadSave_clicked();

	void	upDateFileName(QModelIndex selectedFileIndex);				//called when a file is selected in the FileSave dialog.

signals:
	void	fileNameUpdated(QString newName);


private:
	Ui::FileLoadSave *ui;

	QFileSystemModel	*fileModel;
	QModelIndex			*fileModelIndex;
	QString				filePath;
	QDir				thisDir;
	bool				saveMode;
	QStringList			fileExtensions;

	void	setWorkingDirectory();
	void	populateFileList();

	void	saveFile();
	void	loadFile();


};

#endif // FILELOADSAVE_H
