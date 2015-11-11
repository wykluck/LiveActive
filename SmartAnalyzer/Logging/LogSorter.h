#pragma once
#include <queue>
#include "include\ILogResultProcessor.h"
#include <mutex>
namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogSorter : public ILogResultProcessor
		{
		public:
			LogSorter();
			virtual ~LogSorter();
			virtual void PushFilteredResults(deque<LogEntry>& resultQueue);
			virtual deque<LogEntry> RetrieveResults();

		private:
			deque<LogEntry> m_resultQueue;
			std::mutex m_mtx;
		};
	}
}

