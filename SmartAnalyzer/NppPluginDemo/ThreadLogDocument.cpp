#pragma once
#include "PluginInterface.h"
#include "ThreadLogDocument.h"
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include "menuCmdID.h"
#include <codecvt>

extern NppData nppData;
namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		void ThreadLogDocument::ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue)
		{
			//not supported, just return
			return;
		}

		void ThreadLogDocument::Trace()
		{
			return TraceInternal(m_tracerPtr);
		}
		
		void ThreadLogDocument::UpdateMaps(const Logging::TimeStruct& time, unsigned int lineNo)
		{
			m_lineStructMap.insert(make_pair(lineNo, time));
			//Assign the latest if the time key are the same 
			m_timeLineNumberMap[time] = lineNo;
		}

		unsigned int ThreadLogDocument::FindNearestLine(const Logging::TimeStruct& time)
		{
			auto lowerLineNoItr = m_timeLineNumberMap.lower_bound(time);
			if ((lowerLineNoItr->first != time) && (lowerLineNoItr != m_timeLineNumberMap.begin()))
				lowerLineNoItr--;
			return lowerLineNoItr->second;

		}

		void ThreadLogDocument::SyncTimeWith(LogDocumentBase& other)
		{
			try
			{
				ThreadLogDocument& threadOtherDoc = dynamic_cast<ThreadLogDocument&>(other);
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
				Logging::TimeStruct curTime = lineStructItr->second;

				//find the nearest line in term of time in other view
				unsigned int otherCurLineNo = threadOtherDoc.FindNearestLine(curTime);
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

		std::map<string, shared_ptr<LogDocumentBase>> ThreadLogDocument::Split()
		{
			//not supported, just return
			return std::map<string, shared_ptr<LogDocumentBase>>();
		}

	}
}