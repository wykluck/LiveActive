#pragma once
#include "PluginInterface.h"
#include "MergedLogDocument.h"
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include "ModuleLogDocument.h"
#include "menuCmdID.h"
#include <codecvt>

extern NppData nppData;
namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		std::string prevModuleName;
		void MergedLogDocument::ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue)
		{

			while (!logResultQueue.empty())
			{
				string logStr = logResultQueue.front().GetLogString();
				if (logStr.back() != '\n')
					logStr.append("\n");
				unsigned int curLineNo = ImportFromInternal(curScintilla, logStr);

				const string& curModuleName = logResultQueue.front().GetModuleName();
				if (curModuleName != prevModuleName)
				{
					//create a new LineGroupInfo and insert to the lineGroupModuleIndexMap
					LineGroupModuleInfo lineGroupModuleInfo;
					lineGroupModuleInfo.startLine = lineGroupModuleInfo.endLine = curLineNo;
					lineGroupModuleInfo.moduleName = curModuleName;
					m_lineGroupModuleInfoVec.push_back(lineGroupModuleInfo);
					prevModuleName = lineGroupModuleInfo.moduleName;
				}
				else
				{
					//otherwise, just update the endLine
					m_lineGroupModuleInfoVec.back().endLine = curLineNo;
				}

				logResultQueue.pop_front();
			}
		}

		void MergedLogDocument::Trace()
		{
			auto curScintillaInfo = GetCurrentScintillaInfo();
			//get the current line number
			unsigned int pos = ::SendMessage(curScintillaInfo.handle, SCI_GETCURRENTPOS, 0, 0);
			unsigned int curLineNo = ::SendMessage(curScintillaInfo.handle, SCI_LINEFROMPOSITION, pos, 0) + 1;

			//search lineGroupModuleInfoVec to find the right moduleindex to trace
			for (auto lineGroupModuleInfo : m_lineGroupModuleInfoVec)
			{
				if (curLineNo >= lineGroupModuleInfo.startLine || curLineNo <= lineGroupModuleInfo.endLine)
				{
					auto moduleItr = m_moduleNameTracerMap.find(lineGroupModuleInfo.moduleName);
					if (moduleItr != m_moduleNameTracerMap.end())
					{
						return TraceInternal(moduleItr->second);
					}
				}
			}
		}
		
		void MergedLogDocument::SyncTimeWith(LogDocumentBase& other)
		{
			return;
		}

	
		std::map<string, shared_ptr<LogDocumentBase>> MergedLogDocument::Split()
		{
			//due to the time information is not stored in the MergedLogDocument, not implemment this function.
			std::map<string, shared_ptr<LogDocumentBase>> resultSplitDocMap;
			return resultSplitDocMap;
		}
	}
}