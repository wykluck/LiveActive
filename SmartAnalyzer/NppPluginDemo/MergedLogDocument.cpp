#pragma once
#include "MergedLogDocument.h"
#include "Scintilla.h"

namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		unsigned short prevModuleIndex = 0;
		void MergedLogDocument::ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue)
		{

			while (!logResultQueue.empty())
			{
				string logStr = logResultQueue.front().GetLogString();
				logStr.append("\n");
				unsigned int curLineNo = ImportFromInternal(curScintilla, logStr);

				if (logResultQueue.front().GetModuleIndex() != prevModuleIndex)
				{
					//create a new LineGroupInfo and insert to the lineGroupModuleIndexMap
					LineGroupModuleInfo lineGroupModuleInfo;
					lineGroupModuleInfo.startLine = lineGroupModuleInfo.endLine = curLineNo;
					lineGroupModuleInfo.moduleIndex = logResultQueue.front().GetModuleIndex();
					m_lineGroupModuleInfoVec.push_back(lineGroupModuleInfo);
					prevModuleIndex = lineGroupModuleInfo.moduleIndex;
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
					auto moduleItr = m_moduleIndexTracerMap.find(lineGroupModuleInfo.moduleIndex);
					if (moduleItr != m_moduleIndexTracerMap.end())
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

	
	}
}