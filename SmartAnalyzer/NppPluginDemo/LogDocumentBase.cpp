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
		ScintillaInfo LogDocumentBase::GetCurrentScintillaInfo()
		{
			// Get the current scintilla
			ScintillaInfo scintillaInfo;
			int which = -1;
			::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
			if (which == -1)
				return scintillaInfo;
			scintillaInfo.view = which;
			scintillaInfo.handle = (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;

			
			return scintillaInfo;
		}

		ScintillaInfo LogDocumentBase::GetOtherScintillaInfo()
		{
			// Get the current scintilla
			ScintillaInfo scintillaInfo;
			int which = -1;
			::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
			if (which == -1)
				return scintillaInfo;
			scintillaInfo.view = 1 - which;
			scintillaInfo.handle = (which == 0) ? nppData._scintillaSecondHandle : nppData._scintillaMainHandle;

			return scintillaInfo;
		}

		
		unsigned int LogDocumentBase::ImportFromInternal(HWND curScintilla, const std::string& logStr)
		{
			//go to the end of document and get line number
			unsigned int length = ::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0);
			::SendMessage(curScintilla, SCI_GOTOPOS, length, 0);
			unsigned int pos = ::SendMessage(curScintilla, SCI_GETCURRENTPOS, 0, 0);
			unsigned int curLineNo = ::SendMessage(curScintilla, SCI_LINEFROMPOSITION, pos, 0);

			//Append the logentry to the end of the document
			unsigned int tmpPos = ::SendMessage(curScintilla, SCI_APPENDTEXT, logStr.length(), (LPARAM)logStr.c_str());

			return curLineNo;
		}

		void LogDocumentBase::TraceInternal(std::shared_ptr<LogSourceTracer>& tracerPtr)
		{
			auto curScintillaInfo = GetCurrentScintillaInfo();
			//Get current line into line buffer;
			int lineLength = ::SendMessage(curScintillaInfo.handle, SCI_GETCURLINE, 0, 0);
			char lineBuffer[MAX_LINE_CHARACTERS];
			::SendMessage(curScintillaInfo.handle, SCI_GETCURLINE, lineLength, (LPARAM)lineBuffer);
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