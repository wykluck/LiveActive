#pragma once
#include "LogDocumentBase.h"
#include "LogSourceTracer.h"
#include "TimeStruct.h"
#include <map>
#include <set>
namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		class ThreadLogDocument : public LogDocumentBase
		{
		public:
			ThreadLogDocument(const std::string& moduleName, std::shared_ptr<Logging::LogSourceTracer> tracerPtr)
				: LogDocumentBase(moduleName), m_tracerPtr(tracerPtr)
			{

			}
			virtual ~ThreadLogDocument(){};
			virtual void ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue);
			virtual void Trace();
			virtual void SyncTimeWith(LogDocumentBase& other);
			virtual std::map<string, shared_ptr<LogDocumentBase>> Split();

			void UpdateMaps(const Logging::TimeStruct& time, unsigned int lineNo);
		private:
			unsigned int FindNearestLine(const Logging::TimeStruct& time);
			std::map<Logging::TimeStruct, unsigned int> m_timeLineNumberMap;
			std::map<unsigned int, Logging::TimeStruct> m_lineStructMap;
			std::shared_ptr<Logging::LogSourceTracer> m_tracerPtr;
		};
	}
}