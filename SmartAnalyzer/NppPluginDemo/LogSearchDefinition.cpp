#include "PluginDefinition.h"
#include "menuCmdID.h"

#include <shlwapi.h>
#include <thread>             // std::thread
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include "LogScanner.h"
#include "LogSourceTracer.h"
#include "LogDocumentBase.h"
#include "MergedLogDocument.h"
#include "ModuleLogDocument.h"
#include <codecvt>

using namespace SmartAnalyzer::Logging;

using namespace SmartAnalyzer::NPPLogging;


extern NppData nppData;

std::map<string, shared_ptr<LogDocumentBase>> moduleDocumentMap;

void LogSearchDemo()
{
	

	LogScanner logScanner("D:\\aws\\logs\\Development", "D:\\Github\\LiveActive\\SmartAnalyzer\\Logging\\patterns.json");


	//start a thread to scan the folder for logs
	auto logScan = std::bind(&LogScanner::Scan, &logScanner, "D:\\Github\\LiveActive\\SmartAnalyzer\\Logging\\filter.json", true);
	std::thread	scanThread(logScan);

	scanThread.join();

	//1.according logScanner generate docindex to LogDocumentBase Map
	//2.call to individual LogDocumentBase's importfrom to append logentries
	auto moduleLogResultMap = logScanner.RetrieveResults();
	string logSearchDir = "c:\\Temp\\logsearch\\";
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
	for (auto moduleLogResult : moduleLogResultMap)
	{
		//skip empty result queue
		if (moduleLogResult.second.empty())
			continue;
		
		::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
		auto curScintillaInfo = LogDocumentBase::GetCurrentScintillaInfo();
		
		if (moduleLogResult.first == "all")
		{
			auto insertResult = moduleDocumentMap.insert(make_pair(moduleLogResult.first, shared_ptr<LogDocumentBase>(new MergedLogDocument(moduleLogResult.first,
				logScanner.GetModuleIndexTracerMap()))));
			if (insertResult.second)
				insertResult.first->second->ImportFrom(curScintillaInfo.handle, moduleLogResult.second);
		}
		else
		{	
			auto moduleIndexTraceMap = logScanner.GetModuleIndexTracerMap();
			auto insertResult = moduleDocumentMap.insert(make_pair(moduleLogResult.first, shared_ptr<LogDocumentBase>(new ModuleLogDocument(moduleLogResult.first,
				moduleIndexTraceMap[moduleLogResult.second.front().GetModuleIndex()]))));
			if (insertResult.second)
				insertResult.first->second->ImportFrom(curScintillaInfo.handle, moduleLogResult.second);
		}
		
		//save it to a file named with module name
		string logsearchFilePath = logSearchDir;
		logsearchFilePath.append(moduleLogResult.first);
		
		std::wstring wlogsearchFilePath = utf16conv.from_bytes(logsearchFilePath);
		::SendMessage(nppData._nppHandle, NPPM_SAVECURRENTFILEAS, 0, (LPARAM)wlogsearchFilePath.c_str());

		//set it to readonly
		::SendMessage(curScintillaInfo.handle, SCI_SETREADONLY, 1, 1);
		
	}

	
}


void LogTraceDemo()
{
	TCHAR path[256];
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
	::SendMessage(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)path);
	auto docIndexDocumentItr = moduleDocumentMap.find(utf16conv.to_bytes(path));
	if (docIndexDocumentItr != moduleDocumentMap.end())
		docIndexDocumentItr->second->Trace();

}

void LogSyncDemo()
{
	TCHAR path[256];
	//find the current filename(moduleName) for the current view
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
	::SendMessage(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)path);
	auto moduleDocumentItr = moduleDocumentMap.find(utf16conv.to_bytes(path));

	//find the current filename(moduleName) for another view
	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_VIEW_SWITCHTO_OTHER_VIEW);
	::SendMessage(nppData._nppHandle, NPPM_GETFILENAME, 0, (LPARAM)path);
	auto otherModuleDocumentItr = moduleDocumentMap.find(utf16conv.to_bytes(path));

	::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_VIEW_SWITCHTO_OTHER_VIEW);
	//sync them in time if they are inside the moduleDocumentMap
	if (moduleDocumentItr != moduleDocumentMap.end() && otherModuleDocumentItr != moduleDocumentMap.end())
	{
		moduleDocumentItr->second->SyncTimeWith(*otherModuleDocumentItr->second);
	}
}

