#pragma once
#include "PluginInterface.h"
#include "ModuleLogDocument.h"
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
		void ModuleLogDocument::ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue)
		{

			while (!logResultQueue.empty())
			{
				string logStr = logResultQueue.front().GetLogString();
				if (logStr.back() != '\n')
					logStr.append("\n");
				unsigned int curLineNo = ImportFromInternal(curScintilla, logStr);

				//construct m_lineStructMap
				const string& logThreadId = logResultQueue.front().GetLogThreadId();
				LineStructInfo lineStructInfo = { logResultQueue.front().GetLogTime(), logThreadId };
				m_lineStructMap.insert(make_pair(curLineNo, lineStructInfo));
				//Assign the latest if the time key are the same 
				m_timeLineNumberMap[logResultQueue.front().GetLogTime()] = curLineNo;

				if (m_threadIdSet.find(logThreadId) == m_threadIdSet.end())
				{
					m_threadIdSet.insert(logThreadId);
				}

				logResultQueue.pop_front();
			}
		}

		void ModuleLogDocument::Trace()
		{
			return TraceInternal(m_tracerPtr);
		}
		
		unsigned int ModuleLogDocument::FindNearestLine(const Logging::TimeStruct& time)
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
				Logging::TimeStruct curTime = lineStructItr->second.logTime;

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

		std::map<string, shared_ptr<LogDocumentBase>> ModuleLogDocument::Split()
		{
			std::map<string, shared_ptr<LogDocumentBase>> resultSplitDocMap;
			if (m_threadIdSet.size() <= 1)
			{
				//either no thread or only one thread; no need to split
				return resultSplitDocMap;
			}

			
			auto curScintillaInfo = GetCurrentScintillaInfo();
			int originalDocIndex = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, curScintillaInfo.view);
			int lineCount = ::SendMessage(curScintillaInfo.handle, SCI_GETLINECOUNT, 0, 0);
			char lineBuffer[Logging::MAX_LINE_CHARACTERS];
			string copyBuffer;
			struct ThreadDocStruct
			{
				shared_ptr<ThreadLogDocument> threadDocPtr;
				int docIndex;
			};

			std::map<string, ThreadDocStruct> threadIdDocStructMap;
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> utf16conv;
			//go through the m_lineStructMap to copy the lines related to each thread to different tab
			for (auto lineStructItr = m_lineStructMap.begin(); lineStructItr != m_lineStructMap.end(); lineStructItr++)
			{
				//get the end line number
				auto tempLineStructItr = lineStructItr;
				int endlineNo = lineCount;
				if (++tempLineStructItr != m_lineStructMap.end())
				{
					endlineNo = tempLineStructItr->first;
				}

				//copy line from start to end line to a buffer
				for (auto i = lineStructItr->first; i < endlineNo; i++)
				{
					int copiedCharNum = ::SendMessage(curScintillaInfo.handle, SCI_GETLINE, i, (LPARAM)lineBuffer);
					lineBuffer[copiedCharNum] = '\0';
					copyBuffer.append(lineBuffer);
				}
				

				//move to the correct thread related tab to paste 
				auto threadIdModuleItr = threadIdDocStructMap.find(lineStructItr->second.logThreadId);
				shared_ptr<ThreadLogDocument> threadDocPtr(NULL);
				if (threadIdModuleItr == threadIdDocStructMap.end())
				{
					//not found, create a new tab with thread id appended as tab name	
					::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

					string threadModulePath = LogSaveDir;
					string threadModuleName;
					threadModuleName.append(m_moduleName).append("_").append(lineStructItr->second.logThreadId);
					threadModulePath.append(threadModuleName);
					std::wstring wthreadModulePath = utf16conv.from_bytes(threadModulePath);
					::SendMessage(nppData._nppHandle, NPPM_SAVECURRENTFILEAS, 0, (LPARAM)wthreadModulePath.c_str());

					int curDocIndex = ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTDOCINDEX, 0, curScintillaInfo.view);
					ThreadDocStruct threadDocStruct = { shared_ptr<ThreadLogDocument>(new ThreadLogDocument(threadModuleName, m_tracerPtr)), curDocIndex };

					threadIdDocStructMap.insert(std::make_pair(lineStructItr->second.logThreadId, threadDocStruct));
					threadDocPtr = threadDocStruct.threadDocPtr;
				}
				else
				{
					//found, activate its corresponding doc
					::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, curScintillaInfo.view, threadIdModuleItr->second.docIndex);
					threadDocPtr = threadIdModuleItr->second.threadDocPtr;

				}
				//go to the end of document and get line number
				unsigned int length = ::SendMessage(curScintillaInfo.handle, SCI_GETLENGTH, 0, 0);
				::SendMessage(curScintillaInfo.handle, SCI_GOTOPOS, length, 0);
				unsigned int pos = ::SendMessage(curScintillaInfo.handle, SCI_GETCURRENTPOS, 0, 0);
				unsigned int curLineNo = ::SendMessage(curScintillaInfo.handle, SCI_LINEFROMPOSITION, pos, 0);
				threadDocPtr->UpdateMaps(lineStructItr->second.logTime, curLineNo);

				//append the copybuffer to the target doc
				::SendMessage(curScintillaInfo.handle, SCI_APPENDTEXT, copyBuffer.length(), (LPARAM)copyBuffer.c_str());

				//clear the buffer and reactivate the original tab
				copyBuffer.clear();
				::SendMessage(nppData._nppHandle, NPPM_ACTIVATEDOC, curScintillaInfo.view, originalDocIndex);
			}

			//Insert threadIdDocStructMap to resultSplitDocMap
			for (auto threadIdDocStruct : threadIdDocStructMap)
			{
				resultSplitDocMap.insert(std::make_pair(threadIdDocStruct.second.threadDocPtr->GetModuleName(), threadIdDocStruct.second.threadDocPtr));
			}

			return resultSplitDocMap;
		}

	}
}