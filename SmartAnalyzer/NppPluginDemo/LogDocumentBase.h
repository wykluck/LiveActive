#pragma once
#include <deque>
#include <string>
#include <memory>
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
		class LogDocumentBase
		{
		public:
			LogDocumentBase(int documentIndex) : m_documentIndex(documentIndex)
			{};
			virtual ~LogDocumentBase() = default;

			int GetDocumentIndex()
			{
				return m_documentIndex;
			};
			virtual void ImportFrom(HWND curScintilla, std::deque<SmartAnalyzer::Logging::LogEntry>& logResultQueue) = 0;
			virtual void Trace() = 0;
			virtual void SyncTimeWith(LogDocumentBase& other) = 0;
			static HWND GetCurrentScintilla();
			static HWND GetOtherScintilla();

		protected:
			//append the logStr to curScintilla, and return current line no 
			unsigned int ImportFromInternal(HWND curScintilla, const std::string& logStr);

			//use the tracerPtr to trace to source code using the opencommand
			void TraceInternal(std::shared_ptr<SmartAnalyzer::Logging::LogSourceTracer>& tracerPtr);
			
			int m_documentIndex;
		};
	}
}