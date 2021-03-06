#include "utils.h"

#ifdef __WIN32__
#include "windows.h"
#else
#include <sys/stat.h>
#include <utime.h>
#endif

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/nowide/convert.hpp>

using namespace std;
using namespace boost::posix_time;
using namespace boost;

long Utils::currentMillis()
{
	ptime epoch(boost::gregorian::date(1970,1,1));
	ptime t = microsec_clock::local_time();
	time_duration elapsed = t - epoch;

	return elapsed.total_milliseconds();
}

long Utils::currentSeconds()
{
	time_t now;
	time(&now);

	return now;
}

long Utils::getFileModificationTime(const std::string &filename)
{
#ifdef __WIN32__

	wstring wfilename = nowide::widen(filename);
	HANDLE file = CreateFile(wfilename.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
		return -1;

	FILETIME modTime;

	bool success = GetFileTime(file, NULL, NULL, &modTime);
	CloseHandle(file);

	if (success)
	{
		ptime pt = from_ftime<ptime>(modTime);
		ptime epoch(boost::gregorian::date(1970,1,1));

		return (pt - epoch).total_seconds();
	}
	else
	{
		return -1;
	}
#elif __APPLE__

	struct stat attrib;
	stat(filename.c_str(), &attrib);
	time_t modificationTime = attrib.st_mtimespec.tv_sec;

	return modificationTime;

#else
    struct stat attrib;
    stat(filename.c_str(), &attrib);
    time_t modificationTime = attrib.st_mtim.tv_sec;

    return modificationTime;
#endif
}

long Utils::getFileSize(const string &filename)
{
	system::error_code ec;
	filesystem::path path;

#ifdef __WIN32__

	path = boost::nowide::widen(filename);

#else

	path = filename;

#endif

	uintmax_t fileSize = filesystem::file_size(path, ec);

	if (ec == 0)
		return fileSize;
	else
		return -1;
}

void Utils::touch(const string &filename)
{
#ifdef __WIN32__

	wstring wfilename = nowide::widen(filename);
	HANDLE file = CreateFile(wfilename.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (file == INVALID_HANDLE_VALUE)
		return;

	FILETIME ft;
	SYSTEMTIME st;

	GetSystemTime(&st);
	SystemTimeToFileTime(&st, &ft);
	SetFileTime(file, NULL, NULL, &ft);

	CloseHandle(file);

#else
	struct utimbuf newTime;

	time_t newTimeT;
	time(&newTimeT);

	newTime.actime = newTimeT;
	newTime.modtime = newTimeT;

	utime(filename.c_str(), &newTime);
#endif
}

bool Utils::renameOverwrite(const string &oldName, const string &newName)
{
	filesystem::path o = osPath(oldName);
	filesystem::path n = osPath(newName);
	system::error_code ec;

#ifdef __WIN32__
	system::error_code er;
	if (filesystem::exists(n, er)) {
		filesystem::file_status s = filesystem::status(n);
		bool readOnly = (s.permissions() & filesystem::owner_write) == 0;
		if (readOnly)
			filesystem::permissions(n, filesystem::owner_write);
	}
#endif

	boost::filesystem::rename(o, n, ec);

	return ec == 0;
}

bool Utils::removeFile(const string &path)
{
	filesystem::path p = osPath(path);
	system::error_code ec;

	boost::filesystem::remove(p, ec);

	return ec == 0;
}

filesystem::path Utils::osPath(const string &path)
{
#ifdef __WIN32__
	return filesystem::path(nowide::widen(path));
#else
	return filesystem::path(path);
#endif
}

string Utils::osPath(const filesystem::path &path)
{
#ifdef __WIN32__
	return nowide::narrow(path.generic_wstring());
#else
	return path.generic_string();
#endif
}

void Utils::remove(vector<string> &target, const vector<string> &toRemove)
{
	BOOST_FOREACH (const string &remove, toRemove)
	{
		vector<string>::iterator itr = target.begin();
		while (itr != target.end())
		{
			if (*itr == remove)
				target.erase(itr);
			else
				itr++;
		}
	}
}

