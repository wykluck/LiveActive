#pragma once
#include "LogDocumentBase.h"
#include "LogSourceTracer.h"
#include <map>
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

			
		private:
			struct LineStructInfo
			{
				time_t logTime;
				string logThreadId;
			};

			unsigned int FindNearestLine(time_t time);
			std::map<time_t, unsigned int> m_timeLineNumberMap;
			std::map<unsigned int, LineStructInfo> m_lineStructMap;
			std::shared_ptr<Logging::LogSourceTracer> m_tracerPtr;
		};
	}
}