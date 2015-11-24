#include "PluginInterface.h"
#include "Scintilla.h"
#include "Notepad_plus_msgs.h"
#include "LogDocumentBase.h"
#include "LogSourceTracer.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <codecvt>
#include <boost/process.hpp>
#include <boost/filesystem.hpp>


using namespace boost::process;
using namespace boost::process::initializers;
using namespace boost::filesystem;
using namespace SmartAnalyzer::Logging;

extern NppData nppData;
namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		HWND LogDocumentBase::GetCurrentScintilla()
		{
			// Get the current scintilla
			int which = -1;
			::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
			if (which == -1)
				return (HWND)-1;
			HWND curScintilla = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

			return curScintilla;
		}

		HWND LogDocumentBase::GetOtherScintilla()
		{
			// Get the current scintilla
			int which = -1;
			::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
			if (which == -1)
				return (HWND)-1;
			HWND curScintilla = (which == 0) ? nppData._scintillaSecondHandle : nppData._scintillaMainHandle;

			return curScintilla;
		}

		
		unsigned int LogDocumentBase::ImportFromInternal(HWND curScintilla, const std::string& logStr)
		{
			//Append the logentry to the end of the document
			unsigned int tmpPos = ::SendMessage(curScintilla, SCI_APPENDTEXT, logStr.length(), (LPARAM)logStr.c_str());
			//go to the end of document and get line number 
			unsigned int length = ::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0);
			::SendMessage(curScintilla, SCI_GOTOPOS, length, 0);
			unsigned int pos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
			unsigned int curLineNo = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos, 0);

			return curLineNo;
		}

		void LogDocumentBase::TraceInternal(std::shared_ptr<LogSourceTracer>& tracerPtr)
		{
			HWND curScintilla = GetCurrentScintilla();
			//Get current line into line buffer;
			int lineLength = ::SendMessage(curScintilla, SCI_GETCURLINE, 0, 0);
			char lineBuffer[MAX_LINE_CHARACTERS];
			::SendMessage(curScintilla, SCI_GETCURLINE, lineLength, (LPARAM)lineBuffer);
			string lineStr(lineBuffer);

			LogSourceTracer::TraceResult traceResult = tracerPtr->Trace(lineStr);
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