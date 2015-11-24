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

using namespace SmartAnalyzer::Logging;

using namespace SmartAnalyzer::NPPLogging;


extern NppData nppData;

std::map<int, shared_ptr<LogDocumentBase>> docIndexDocumentMap;

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
	for (auto moduleLogResult : moduleLogResultMap)
	{
		//skip empty result queue
		if (moduleLogResult.second.empty())
			continue;
		
		::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);
		HWND curScintilla = LogDocumentBase::GetCurrentScintilla();
		int currentDocIndex = (int)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, (LPARAM)curScintilla);
		
		if (moduleLogResult.first == "all")
		{
			auto insertResult = docIndexDocumentMap.insert(make_pair(currentDocIndex, shared_ptr<LogDocumentBase>(new MergedLogDocument(currentDocIndex,
				logScanner.GetModuleIndexTracerMap()))));
			if (insertResult.second)
				insertResult.first->second->ImportFrom(curScintilla, moduleLogResult.second);
		}
		else
		{	
			auto moduleIndexTraceMap = logScanner.GetModuleIndexTracerMap();
			auto insertResult = docIndexDocumentMap.insert(make_pair(currentDocIndex, shared_ptr<LogDocumentBase>(new ModuleLogDocument(currentDocIndex,
				moduleIndexTraceMap[moduleLogResult.second.front().GetModuleIndex()]))));
			if (insertResult.second)
				insertResult.first->second->ImportFrom(curScintilla, moduleLogResult.second);
		}
		
		//set it to readonly
		::SendMessage(curScintilla, SCI_SETREADONLY, 1, 1);
	}

	
}


void LogTraceDemo()
{
	HWND curScintilla = LogDocumentBase::GetCurrentScintilla();
	int currentDocIndex = (int)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, (LPARAM)curScintilla);
	auto docIndexDocumentItr = docIndexDocumentMap.find(currentDocIndex);
	if (docIndexDocumentItr != docIndexDocumentMap.end())
		docIndexDocumentItr->second->Trace();

}

void LogSyncDemo()
{
	HWND curScintilla = LogDocumentBase::GetCurrentScintilla();
	int currentDocIndex = (int)::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, (LPARAM)curScintilla);
	auto docIndexDocumentItr = docIndexDocumentMap.find(currentDocIndex);
	if (docIndexDocumentItr != docIndexDocumentMap.end())
	{
		auto otherDocIndexDocumentItr = docIndexDocumentItr;
		otherDocIndexDocumentItr++;
		if (otherDocIndexDocumentItr == docIndexDocumentMap.end())
		{
			otherDocIndexDocumentItr = docIndexDocumentMap.begin();
		}
		docIndexDocumentItr->second->SyncTimeWith(*otherDocIndexDocumentItr->second);
	}
}

