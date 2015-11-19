#include "PluginDefinition.h"
#include "menuCmdID.h"

#include <shlwapi.h>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include "LogScanner.h"
#include "LogSourceTracer.h"
#include <boost/process.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <codecvt>
using namespace SmartAnalyzer::Logging;
using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::filesystem;

extern NppData nppData;

struct LineGroupModuleInfo
{
	unsigned int startLine;
	unsigned int endLine;
	unsigned short moduleIndex;
};


const int MAX_LINE_LEGNTH = 256;
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
		//Append the logentry to the end of the document
		unsigned int tmpPos = ::SendMessage(curScintilla, SCI_APPENDTEXT, outputStr.length(), (LPARAM)outputStr.c_str());
		//go to the end of document and get line number 
		unsigned int length = ::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0);
		::SendMessage(curScintilla, SCI_GOTOPOS, length, 0);
		unsigned int pos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
		unsigned int curLineNo = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos, 0);
		if (resultQueue.front().GetModuleIndex() != prevModuleIndex)
		{
			//create a new LineGroupInfo and insert to the lineGroupModuleIndexMap
			LineGroupModuleInfo lineGroupModuleInfo;
			lineGroupModuleInfo.startLine = lineGroupModuleInfo.endLine = curLineNo;
			lineGroupModuleInfo.moduleIndex = resultQueue.front().GetModuleIndex();
			lineGroupModuleInfoVec.push_back(lineGroupModuleInfo);
			prevModuleIndex = lineGroupModuleInfo.moduleIndex;
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
	//::SendMessage(curScintilla, SCI_SETREADONLY, 1, 1);

	LogScanner logScanner("D:\\aws\\logs\\Development", "D:\\Github\\LiveActive\\SmartAnalyzer\\Logging\\patterns.json");

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
	char lineBuffer[MAX_LINE_LEGNTH];
	::SendMessage(curScintilla, SCI_GETCURLINE, lineLength, (LPARAM)lineBuffer);
	//get the current line number
	unsigned int pos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
	unsigned int curLineNo = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos, 0) + 1;
	string lineStr(lineBuffer);
	//search lineGroupModuleInfoVec to find the right moduleindex to trace
	
	for (auto lineGroupModuleInfo : lineGroupModuleInfoVec)
	{
		if (curLineNo >= lineGroupModuleInfo.startLine || curLineNo <= lineGroupModuleInfo.endLine)
		{
			auto moduleItr = moduleIndexTracerMap.find(lineGroupModuleInfo.moduleIndex);
			if (moduleItr != moduleIndexTracerMap.end())
			{
				LogSourceTracer::TraceResult traceResult = moduleItr->second->Trace(lineStr);
				if (traceResult.m_bSuccess)
				{
					vector<string> argVec;
			
					boost::split(argVec, traceResult.subResultVec[0].m_openArgs, boost::is_any_of(" \t"));
					argVec.insert(argVec.begin(), traceResult.subResultVec[0].m_openCommand);
					vector<wstring> wArgVec;
					for (auto arg : argVec)
					{
						
						std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
						std::wstring utf16 = utf16conv.from_bytes(arg);
						wArgVec.push_back(utf16);
					}
					execute(set_args(wArgVec));
				}
				
				return;
			}
		}
	}
	

}

