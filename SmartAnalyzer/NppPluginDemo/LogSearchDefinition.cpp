#include "PluginDefinition.h"
#include "menuCmdID.h"

#include <shlwapi.h>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include "LogScanner.h"
using namespace SmartAnalyzer::Logging;

extern NppData nppData;

void RetrieveAndOutputResults(HWND curScintilla, LogScanner& logScanner)
{
	deque<LogEntry> resultQueue = logScanner.RetrieveResults();
	while (!resultQueue.empty())
	{
		string outputStr = resultQueue.front().GetLogString();
		outputStr.append("\n");
		::SendMessage(curScintilla, SCI_APPENDTEXT, outputStr.length(), (LPARAM)outputStr.c_str());
		resultQueue.pop_front();
	}

}

void LogSearchDemo()
{
	// Open a new document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

	// Get the current scintilla
	int which = -1;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return;
	HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;


	LogScanner logScanner("D:\\aws\\logs", "patterns.json");

	//start a thread to scan the folder for logs
	auto logScan = std::bind(&LogScanner::Scan, &logScanner, 50, true);
	std::thread	scanThread(logScan);

	//wait for logs to be retrieved and output to the scintilla
	while (!logScanner.IsFinished())
	{
		RetrieveAndOutputResults(curScintilla, logScanner);
	}
	RetrieveAndOutputResults(curScintilla, logScanner);
	scanThread.join();
}

