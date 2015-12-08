#pragma once
#include <deque>
#include <string>
#include <memory>
#include <map>
#include "LogEntry.h"
#include <shlwapi.h>

namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogSourceTracer;
	}
	namespace NPPLogging
	{
		struct ScintillaInfo
		{
			ScintillaInfo() : view(-1), handle(NULL) {}
			int view;
			HWND handle;
		};

		class LogDocumentBase
		{
		public:
			LogDocumentBase(const std::string& moduleName) : m_moduleName(moduleName)
			{};
			virtual ~LogDocumentBase() = default;

			
			virtual void ImportFrom(HWND curScintilla, std::deque<SmartAnalyzer::Logging::LogEntry>& logResultQueue) = 0;
			virtual void Trace() = 0;
			virtual void SyncTimeWith(LogDocumentBase& other) = 0;
			virtual std::map<std::string, std::shared_ptr<LogDocumentBase>> Split() = 0;

			static ScintillaInfo GetCurrentScintillaInfo();
			static ScintillaInfo GetOtherScintillaInfo();

			const std::string& GetModuleName()
			{
				return m_moduleName;
			}
		protected:
			//append the logStr to curScintilla, and return current line no 
			unsigned int ImportFromInternal(HWND curScintilla, const std::string& logStr);

			//use the tracerPtr to trace to source code using the opencommand
			void TraceInternal(std::shared_ptr<SmartAnalyzer::Logging::LogSourceTracer>& tracerPtr);
			
			std::string m_moduleName;
		};

		static std::string LogSaveDir = "c:\\Temp\\logsave\\";
	}
}