#pragma once
#include "PluginInterface.h"
#include "ModuleLogDocument.h"
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include "menuCmdID.h"

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
				//Assign the latest if the time key are the same 
				m_timeLineNumberMap[logResultQueue.front().GetLogTime()] = curLineNo;

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
			if ((lowerLineNoItr->first != time) && (lowerLineNoItr != m_timeLineNumberMap.begin()))
				lowerLineNoItr--;
			return lowerLineNoItr->second;

		}

		void ModuleLogDocument::SyncTimeWith(LogDocumentBase& other)
		{
			try
			{
				ModuleLogDocument& moduleOtherDoc = dynamic_cast<ModuleLogDocument&>(other);
				//get the time for the current line
				auto curScintillaInfo = GetCurrentScintillaInfo();
				
				//get the current line's time
				unsigned int pos = ::SendMessage(curScintillaInfo.handle, SCI_GETCURRENTPOS, 0, 0);
				unsigned int curLineNo = ::SendMessage(curScintillaInfo.handle, SCI_LINEFROMPOSITION, pos, 0);
				int displayLineNo = ::SendMessage(curScintillaInfo.handle, SCI_VISIBLEFROMDOCLINE, curLineNo, 0);
				::SendMessage(curScintillaInfo.handle, SCI_SETFIRSTVISIBLELINE, displayLineNo, 0);
				//find the line has timestamp on it
				auto lineStructItr = m_lineStructMap.lower_bound(curLineNo);
				if (lineStructItr->first != curLineNo)
					lineStructItr--;
				time_t curTime = lineStructItr->second.logTime;
				//TODO: Scroll the current line to center of the view

				//find the nearest line in term of time in other view
				unsigned int otherCurLineNo = moduleOtherDoc.FindNearestLine(curTime);
				auto otherScintillaInfo = GetOtherScintillaInfo();
				::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_VIEW_SWITCHTO_OTHER_VIEW);
				::SendMessage(otherScintillaInfo.handle, SCI_GOTOLINE, otherCurLineNo, 0);
				int otherDisplayLineNo = ::SendMessage(otherScintillaInfo.handle, SCI_VISIBLEFROMDOCLINE, otherCurLineNo, 0);
				::SendMessage(otherScintillaInfo.handle, SCI_SETFIRSTVISIBLELINE, otherDisplayLineNo, 0);

				//restore the current document index in the current view
				::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_VIEW_SWITCHTO_OTHER_VIEW);
				
			}
			catch (std::bad_cast&)
			{
				//other is not ModuleLogDocument, can't sync and just return
				return;
			}
			
		}

	
	}
}