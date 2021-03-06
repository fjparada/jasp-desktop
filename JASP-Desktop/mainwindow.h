#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>

#include "dataset.h"

#include "datasettablemodel.h"
#include "enginesync.h"
#include "analyses.h"
#include "widgets/progresswidget.h"

#include "analysisforms/analysisform.h"
#include "asyncloader.h"
#include "optionsform.h"
#include "activitylog.h"
#include "fileevent.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	void open(QString filepath);
	~MainWindow();

protected:
	virtual void resizeEvent(QResizeEvent *event) OVERRIDE;
	virtual void dragEnterEvent(QDragEnterEvent *event) OVERRIDE;
	virtual void dropEvent(QDropEvent *event) OVERRIDE;
	virtual void closeEvent(QCloseEvent *event) OVERRIDE;

private:
	Ui::MainWindow *ui;

	AnalysisForm *_currentOptionsWidget;
	QMenu* _analysisMenu;
	DataSetPackage *_package;
	DataSetTableModel *_tableModel;
	Analysis *_currentAnalysis;

	double _webViewZoom;

	Analyses *_analyses;
	EngineSync* _engineSync;

	void packageChanged(DataSetPackage *package);

	bool closeRequestCheck(bool &isSaving);

	AsyncLoader _loader;
	ProgressWidget *_progressIndicator;

	bool _inited;
	bool _applicationExiting = false;

	AnalysisForm* loadForm(Analysis *analysis);
	void showForm(Analysis *analysis);
	void closeCurrentOptionsWidget();
	void removeAnalysis(Analysis *analysis);

	QWidget *_buttonPanel;
	QVBoxLayout *_buttonPanelLayout;
	QPushButton *_okButton;
	QPushButton *_runButton;
	QPushButton *_menuButton;

	OptionsForm *_optionsForm;

	std::map<std::string, AnalysisForm *> _analysisForms;

	int _tableViewWidthBeforeOptionsMadeVisible;

	bool _resultsViewLoaded = false;
	bool _openedUsingArgs = false;
	QString _openOnLoadFilename;
	QSettings _settings;
	ActivityLog *_log;
	QString _fatalError;

	QString escapeJavascriptString(const QString &str);
	void getAnalysesNotes();
	Json::Value getResultsMeta();

signals:
	void analysisSelected(int id);
	void analysisUnselected();
	void analysisChangedDownstream(int id, QString options);
	void saveTextToFile(QString filename, QString text);
	void pushToClipboard(QString mimeType, QString data, QString html);
	void pushImageToClipboard(QByteArray base64, QString html);
	void saveTempImage(int id, QString path, QByteArray data);
	void showAnalysesMenu(QString options);
	void removeAnalysisRequest(int id);
	void updateNote(int id, QString key);
	void updateAnalysesNotes(QString notes);
	void simulatedMouseClick(int x, int y, int count);
	void resultsDocumentChanged();

private slots:

	void analysisResultsChangedHandler(Analysis* analysis);
	void analysisNotesLoadedHandler(Analysis *analysis);
	void analysisSelectedHandler(int id);
	void analysisUnselectedHandler();
	void pushImageToClipboardHandler(const QByteArray &base64, const QString &html);
	void saveTextToFileHandler(const QString &filename, const QString &data);
	void pushToClipboardHandler(const QString &mimeType, const QString &data, const QString &html);
	void saveTempImageHandler(int id, QString path, QByteArray data);
	void analysisChangedDownstreamHandler(int id, QString options);

	void resultsDocumentChangedHandler();
	void simulatedMouseClickHandler(int x, int y, int count);
	void updateNoteHandler(int id, QString key);
	void removeAnalysisRequestHandler(int id);
	void showAnalysesMenuHandler(QString options);
	void removeSelected();
	void editTitleSelected();
	void copySelected();
	void citeSelected();
	void noteSelected();
	void menuHidding();

	void tabChanged(int index);
	void helpToggled(bool on);
	void dataSetIORequest(FileEvent *event);
	void dataSetIOCompleted(FileEvent *event);
	void populateUIfromDataSet();
	void itemSelected(const QString &item);
	void exportSelected(const QString &filename);

	void adjustOptionsPanelWidth();
	void splitterMovedHandler(int, int);

	void hideOptionsPanel();
	void showOptionsPanel();
	void showTableView();
	void hideTableView();

	void analysisOKed();
	void analysisRunned();
	void analysisRemoved();

	void updateMenuEnabledDisabledStatus();
	void updateUIFromOptions();

	void resultsPageLoaded(bool success);
	void scrollValueChangedHandle();

	void saveKeysSelected();
	void openKeysSelected();

	void illegalOptionStateChanged();
	void fatalError();

	void helpFirstLoaded(bool ok);
	void requestHelpPage(const QString &pageName);
};

#endif // MAINWIDGET_H
