#pragma once
#include <deque>
#include <string>
#include "LogEntry.h"
using namespace std;
namespace SmartAnalyzer
{
	namespace Logging
	{
		class ILogResultProcessor
		{
		public:
			virtual void PushFilteredResults(deque<LogEntry>& resultQueue) = 0;
			virtual deque<LogEntry> RetrieveResults() = 0;
		};
	}
}
