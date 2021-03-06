
#include "fsbmrecent.h"

#include <QStringList>
#include <QFileInfo>
#include <QEvent>
#include <QDebug>

FSBMRecent::FSBMRecent(QObject *parent)
	: FSBModel(parent)
{
	parent->installEventFilter(this);
}

void FSBMRecent::refresh()
{
	populate(load());
}

bool FSBMRecent::eventFilter(QObject *object, QEvent *event)
{
	if (event->type() == QEvent::Show || event->type() == QEvent::WindowActivate)
		refresh();

	return QObject::eventFilter(object, event);
}

void FSBMRecent::addRecent(const QString &path)
{
	QStringList recents = load();
	recents.removeAll(path);

	recents.prepend(path);
	while (recents.size() > 5)
		recents.removeLast();

	_settings.setValue("recentItems", recents);
	_settings.sync();

	populate(recents);
}


void FSBMRecent::populate(const QStringList &paths)
{
	_entries.clear();

	for (int i = 0; i < 5 && i < paths.length(); i++)
	{
		QString path = paths.at(i);

		FSEntry::EntryType entryType = FSEntry::Other;
		if (path.endsWith(".jasp", Qt::CaseInsensitive))
			entryType = FSEntry::JASP;

		FSEntry entry = createEntry(path, entryType);

		_entries.append(entry);
	}

	emit entriesChanged();
}

QStringList FSBMRecent::load()
{
	_settings.sync();

	QVariant v = _settings.value("recentItems");
	if (v.type() != QVariant::StringList && v.type() != QVariant::String)
	{
		// oddly, under linux, loading a setting value of type StringList which has
		// only a single string in it, gives you just a string. we QVariant::String is acceptable too

		qDebug() << "BackStageForm::loadRecents();  setting 'recentItems' is not a QStringList";
		return QStringList();
	}

	QStringList recents = v.toStringList();

	for (int i = 0; i < recents.size(); i++)
	{
		if ( ! QFileInfo::exists(recents[i]))
		{
			recents.removeAt(i);
			i -= 1;
		}
	}

	return recents;
}

