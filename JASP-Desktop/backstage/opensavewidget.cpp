
#include "opensavewidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QFileInfo>

#include "fsbmrecent.h"

OpenSaveWidget::OpenSaveWidget(QWidget *parent) : QWidget(parent)
{
	_mode = FileEvent::FileOpen;
	_currentFileHasPath = false;
	_currentFileReadOnly = false;

	QGridLayout *layout = new QGridLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	setLayout(layout);

	_tabWidget = new VerticalTabWidget(this);

	QWidget *webWidget = new QWidget(this);
	//webWidget->setMinimumWidth(400);
	webWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	/*QGridLayout *webWidgetLayout = new QGridLayout(webWidget);
	webWidgetLayout->setMargin(36);

	QFrame *webFrame = new QFrame(webWidget);
	QGridLayout *webLayout = new QGridLayout(webFrame);
	webFrame->setLayout(webLayout);
	webLayout->setMargin(0);
	webFrame->setFrameShape(QFrame::Box);
	webFrame->setFrameStyle(QFrame::Panel);
	webFrame->setLineWidth(1);
	webFrame->setMinimumWidth(200);

	QWebView *webView = new QWebView(webFrame);
	webLayout->addWidget(webView);

	webWidgetLayout->addWidget(webFrame);*/

	layout->addWidget(_tabWidget, 0, 0);
	layout->addWidget(webWidget, 0, 1);

	_fsmRecent   = new FSBMRecent(this);
	_fsmExamples = new FSBMExamples(this);

	_bsRecent = new FSBrowser(_tabWidget);
	_bsRecent->setFSModel(_fsmRecent);

	_bsComputer = new BackstageComputer(_tabWidget);
#ifdef QT_DEBUG
	_bsOSF = new BackstageOSF(_tabWidget);
#endif

	_bsExamples = new FSBrowser(_tabWidget);
	_bsExamples->setFSModel(_fsmExamples);

	_tabWidget->addTab(_bsRecent, "Recent");
	_tabWidget->addTab(_bsComputer, "Computer");
#ifdef QT_DEBUG
	_tabWidget->addTab(_bsOSF, "OSF", QIcon(":/icons/logo-osf.png"));
#endif
	_tabWidget->addTab(_bsExamples, "Examples");

	connect(_bsRecent, SIGNAL(entryOpened(QString)), this, SLOT(dataSetOpenRequestHandler(QString)));
	connect(_bsComputer, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));
#ifdef QT_DEBUG
	connect(_bsOSF, SIGNAL(dataSetIORequest(FileEvent *)), this, SLOT(dataSetIORequestHandler(FileEvent *)));
#endif
	connect(_bsExamples, SIGNAL(entryOpened(QString)), this, SLOT(dataSetOpenExampleRequestHandler(QString)));
}

VerticalTabWidget *OpenSaveWidget::tabWidget()
{
	return _tabWidget;
}

void OpenSaveWidget::setSaveMode(FileEvent::FileMode mode)
{
	_mode = mode;

	_bsComputer->setMode(_mode);

	if (_mode == FileEvent::FileOpen)
	{
		_tabWidget->showTab(_bsRecent);
		_tabWidget->showTab(_bsExamples);
	}
	else
	{
		_tabWidget->hideTab(_bsRecent);
		_tabWidget->hideTab(_bsExamples);
	}
}

FileEvent *OpenSaveWidget::open()
{
	FileEvent *event = _bsComputer->browseOpen();
	if ( ! event->isCompleted())
		dataSetIORequestHandler(event);
	return event;
}

FileEvent *OpenSaveWidget::open(const QString &path)
{
	FileEvent *event = new FileEvent(this);
	event->setOperation(FileEvent::FileOpen);
	event->setPath(path);

	if ( ! path.endsWith(".jasp", Qt::CaseInsensitive))
		event->setReadOnly();

	dataSetIORequestHandler(event);

	return event;
}

FileEvent *OpenSaveWidget::save()
{
	FileEvent *event;

	if (_currentFileHasPath == false || _currentFileReadOnly)
	{
		event = _bsComputer->browseSave();
		if (event->isCompleted())
			return event;
	}
	else
	{
		event = new FileEvent(this);
		event->setOperation(FileEvent::FileSave);
		event->setPath(_currentFilePath);
	}

	dataSetIORequestHandler(event);

	return event;
}

FileEvent *OpenSaveWidget::close()
{
	FileEvent *event = new FileEvent(this);
	event->setOperation(FileEvent::FileClose);
	dataSetIORequestHandler(event);

	return event;
}

void OpenSaveWidget::dataSetIOCompleted(FileEvent *event)
{
	if (event->operation() == FileEvent::FileSave || event->operation() == FileEvent::FileOpen)
	{
		if (event->successful())
		{
			_fsmRecent->addRecent(event->path());
			_bsComputer->addRecent(event->path());

			// all this stuff is a hack
			QFileInfo info(event->path());
			_bsComputer->setFileName(info.baseName());

			_currentFilePath = event->path();
			_currentFileHasPath = true;
			_currentFileReadOnly = event->isReadOnly();
		}
	}
	else if (event->operation() == FileEvent::FileClose)
	{
		_bsComputer->clearFileName();

		_currentFileHasPath = false;
		_currentFilePath = "";
	}
}

void OpenSaveWidget::dataSetIORequestHandler(FileEvent *event)
{
	connect(event, SIGNAL(completed(FileEvent*)), this, SLOT(dataSetIOCompleted(FileEvent*)));
	emit dataSetIORequest(event);
}

void OpenSaveWidget::dataSetOpenRequestHandler(QString path)
{
	open(path);
}

void OpenSaveWidget::dataSetOpenExampleRequestHandler(QString path)
{
	FileEvent *event = new FileEvent(this);
	event->setPath(path);
	event->setReadOnly();

	dataSetIORequestHandler(event);
}

