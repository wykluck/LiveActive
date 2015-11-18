#include "PluginDefinition.h"
#include "menuCmdID.h"

#include <shlwapi.h>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include "LogScanner.h"
#include "LogSourceTracer.h"
using namespace SmartAnalyzer::Logging;

extern NppData nppData;

struct LineGroupModuleInfo
{
	unsigned int startLine;
	unsigned int endLine;
	unsigned short moduleIndex;
};



std::vector<LineGroupModuleInfo> lineGroupModuleInfoVec;
std::map<unsigned short, std::shared_ptr<LogSourceTracer>> moduleIndexTracerMap;
unsigned short prevModuleIndex = 0;

void RetrieveAndOutputResults(HWND curScintilla, LogScanner& logScanner)
{
	deque<LogEntry> resultQueue = logScanner.RetrieveResults();

	while (!resultQueue.empty())
	{
		string outputStr = resultQueue.front().GetLogString();
		outputStr.append("\n");
		//get the current line number
		unsigned int curLineNo = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0);
		::SendMessage(curScintilla, SCI_APPENDTEXT, outputStr.length(), (LPARAM)outputStr.c_str());
		if (resultQueue.front().GetModuleIndex() != prevModuleIndex)
		{
			//create a new LineGroupInfo and insert to the lineGroupModuleIndexMap
			LineGroupModuleInfo lineGroupModuleInfo;
			lineGroupModuleInfo.startLine = lineGroupModuleInfo.endLine = curLineNo;
			lineGroupModuleInfo.moduleIndex = resultQueue.front().GetModuleIndex();
			lineGroupModuleInfoVec.push_back(lineGroupModuleInfo);
		}
		else
		{
			//otherwise, just update the endLine
			lineGroupModuleInfoVec.back().endLine = curLineNo;
		}

		resultQueue.pop_front();
	}

}

HWND GetCurrentScintilla()
{
	// Get the current scintilla
	int which = -1;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
	if (which == -1)
		return (HWND)-1;
	HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

	return curScintilla;
}

void LogSearchDemo()
{
	// Open a new document
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
	

	HWND curScintilla = GetCurrentScintilla();

	//set it to readonly
	::SendMessage(curScintilla, SCI_SETREADONLY, 1, 1);

	LogScanner logScanner("D:\\aws\\logs", "D:\\Github\\LiveActive\\SmartAnalyzer\\Logging\\patterns.json");

	moduleIndexTracerMap = logScanner.GetModuleIndexTracerMap();

	//start a thread to scan the folder for logs
	auto logScan = std::bind(&LogScanner::Scan, &logScanner, "D:\\Github\\LiveActive\\SmartAnalyzer\\Logging\\filter.json", 50, true);
	std::thread	scanThread(logScan);

	//wait for logs to be retrieved and output to the scintilla
	while (!logScanner.IsFinished())
	{
		RetrieveAndOutputResults(curScintilla, logScanner);
	}
	RetrieveAndOutputResults(curScintilla, logScanner);
	scanThread.join();
}

void LogTraceDemo()
{
	HWND curScintilla = GetCurrentScintilla();
	//Get current line into line buffer;
	int lineLength = ::SendMessage(curScintilla, SCI_GETCURLINE, 0, 0);
	char* lineBuffer = new char[lineLength + 1];
	::SendMessage(curScintilla, SCI_GETCURLINE, lineLength, (LPARAM)lineBuffer);
	//get the current line number
	unsigned int curLineNo = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTLINE, 0, 0);
	string lineStr(lineBuffer);
	delete lineBuffer;
	//search lineGroupModuleInfoVec to find the right moduleindex to trace
	for (auto lineGroupModuleInfo : lineGroupModuleInfoVec)
	{
		if (curLineNo >= lineGroupModuleInfo.startLine || curLineNo <= lineGroupModuleInfo.endLine)
		{
			auto moduleItr = moduleIndexTracerMap.find(lineGroupModuleInfo.moduleIndex);
			if (moduleItr != moduleIndexTracerMap.end())
			{
				moduleItr->second->Trace(lineStr);
			}
		}
	}
}

