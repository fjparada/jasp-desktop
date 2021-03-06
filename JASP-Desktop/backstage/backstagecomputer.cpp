
#include "backstagecomputer.h"
#include "ui_backstagecomputer.h"

#include <QFileDialog>

#include "fsbmrecentfolders.h"


BackstageComputer::BackstageComputer(QWidget *parent) :
	BackstagePage(parent),
	ui(new Ui::BackstageComputer)
{
	_hasFileName = false;

	ui->setupUi(this);

	_model = new FSBMRecentFolders(this);

	ui->browser->setBrowseMode(FSBrowser::BrowseOpenFolder);
	ui->browser->setViewType(FSBrowser::ListView);
	ui->browser->setFSModel(_model);

	installEventFilter(this);

	connect(ui->browser, SIGNAL(entryOpened(QString)), this, SLOT(selectionMade(QString)));
	connect(ui->browseButton, SIGNAL(clicked(bool)), this, SLOT(browseSelected()));
}

BackstageComputer::~BackstageComputer()
{
	delete ui;
}

FileEvent *BackstageComputer::browseOpen(const QString &path)
{
	QString browsePath;
	if (path == "")
		browsePath = _model->mostRecent();
	else
		browsePath = path;

#ifdef QT_DEBUG
	QString finalPath = QFileDialog::getOpenFileName(this, "Open", browsePath, "Data Sets (*.jasp *.csv *.spss)");
#else
	QString finalPath = QFileDialog::getOpenFileName(this, "Open", browsePath, "Data Sets (*.jasp *.csv)");
#endif

	FileEvent *event = new FileEvent(this);
	event->setOperation(FileEvent::FileOpen);

	if (finalPath != "")
	{
		event->setPath(finalPath);

		if ( ! path.endsWith(".jasp", Qt::CaseInsensitive))
			event->setReadOnly();

		emit dataSetIORequest(event);
	}
	else
	{
		event->setComplete(false);
	}

	return event;
}

FileEvent *BackstageComputer::browseSave(const QString &path)
{
	QString browsePath = path;
	if (path == "")
		browsePath = _model->mostRecent();

	if (_hasFileName)
		browsePath += QDir::separator() + _fileName;

	QString finalPath = QFileDialog::getSaveFileName(this, "Save", browsePath, "JASP Files (*.jasp)");

	FileEvent *event = new FileEvent(this);
	event->setOperation(FileEvent::FileSave);

	if (finalPath != "")
	{
		event->setPath(finalPath);
		emit dataSetIORequest(event);
	}
	else
	{
		event->setComplete(false);
	}

	return event;
}

void BackstageComputer::addRecent(const QString &path)
{
	_model->addRecent(path);
}

void BackstageComputer::setFileName(const QString &filename)
{
	_fileName = filename;
	_hasFileName = true;
}

void BackstageComputer::clearFileName()
{
	_hasFileName = false;
}

bool BackstageComputer::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::Show || event->type() == QEvent::WindowActivate)
		_model->refresh();

	return QWidget::eventFilter(object, event);
}

void BackstageComputer::selectionMade(QString path)
{
	if (_mode == FileEvent::FileOpen)
		browseOpen(path);
	else
		browseSave(path);
}

void BackstageComputer::browseSelected()
{
	selectionMade(_model->mostRecent());
}
