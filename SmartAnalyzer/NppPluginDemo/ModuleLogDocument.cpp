#pragma once
#include "PluginInterface.h"
#include "ModuleLogDocument.h"
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"

extern NppData nppData;
namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		void ModuleLogDocument::ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue)
		{

			while (!logResultQueue.empty())
			{
				string logStr = logResultQueue.front().GetLogString();
				logStr.append("\n");
				unsigned int curLineNo = ImportFromInternal(curScintilla, logStr);

				//construct m_lineStructMap
				LineStructInfo lineStructInfo = { logResultQueue.front().GetLogTime(), logResultQueue.front().GetLogThreadId() };
				m_lineStructMap.insert(make_pair(curLineNo, lineStructInfo));
				m_timeLineNumberMap.insert(make_pair(logResultQueue.front().GetLogTime(), curLineNo));

				logResultQueue.pop_front();
			}
		}

		void ModuleLogDocument::Trace()
		{
			return TraceInternal(m_tracerPtr);
		}
		
		unsigned int ModuleLogDocument::FindNearestLine(time_t time)
		{
			auto lowerLineNoItr = m_timeLineNumberMap.lower_bound(time);
			auto upperLineNoItr = lowerLineNoItr++;
			if (upperLineNoItr == m_timeLineNumberMap.end() || (abs(lowerLineNoItr->first - time) <= abs(upperLineNoItr->first - time)))
				return lowerLineNoItr->second;
			else
				return upperLineNoItr->second;

		}

		void ModuleLogDocument::SyncTimeWith(LogDocumentBase& other)
		{
			try
			{
				ModuleLogDocument& moduleOtherDoc = dynamic_cast<ModuleLogDocument&>(other);
				//get the time for the current line
				HWND curScintilla = GetCurrentScintilla();
				unsigned int pos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
				unsigned int curLineNo = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos, 0) + 1;
				auto lineStructItr = m_lineStructMap.find(curLineNo);
				time_t curTime = lineStructItr->second.logTime;
				//TODO: Scroll the current line to center of the view

				HWND otherScintilla = GetOtherScintilla();
				::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, (WPARAM)otherScintilla, moduleOtherDoc.GetDocumentIndex());
				unsigned int otherCurLineNo = moduleOtherDoc.FindNearestLine(curTime);
				::SendMessage(otherScintilla, SCI_GOTOLINE, otherCurLineNo - 1, 0);

				::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, (WPARAM)curScintilla, m_documentIndex);
			}
			catch (std::bad_cast&)
			{
				//other is not ModuleLogDocument, can't sync and just return
				return;
			}
			
		}

	
	}
}