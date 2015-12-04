#pragma once
#include "MergedLogDocument.h"
#include "Scintilla.h"

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
				logStr.append("\n");
				unsigned int curLineNo = ImportFromInternal(curScintilla, logStr);

				if (logResultQueue.front().GetModuleName() != prevModuleName)
				{
					//create a new LineGroupInfo and insert to the lineGroupModuleIndexMap
					LineGroupModuleInfo lineGroupModuleInfo;
					lineGroupModuleInfo.startLine = lineGroupModuleInfo.endLine = curLineNo;
					lineGroupModuleInfo.moduleName = logResultQueue.front().GetModuleName();
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

	
		void MergedLogDocument::Split()
		{
			//TODO: To be implemented

			return;
		}
	}
}