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
		class ModuleLogDocument : public LogDocumentBase
		{
		public:
			ModuleLogDocument(const std::string& moduleName, std::shared_ptr<Logging::LogSourceTracer> tracerPtr)
				: LogDocumentBase(moduleName), m_tracerPtr(tracerPtr)
			{

			}
			virtual ~ModuleLogDocument(){};
			virtual void ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue);
			virtual void Trace();
			virtual void SyncTimeWith(LogDocumentBase& other);
			virtual void Split();
			
			
		private:
			struct LineStructInfo
			{
				Logging::TimeStruct logTime;
				string logThreadId;
			};

			unsigned int FindNearestLine(const Logging::TimeStruct& time);
			std::map<Logging::TimeStruct, unsigned int> m_timeLineNumberMap;
			std::map<unsigned int, LineStructInfo> m_lineStructMap;
			std::shared_ptr<Logging::LogSourceTracer> m_tracerPtr;
			std::set<string> m_threadIdSet;
		};
	}
}