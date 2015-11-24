#pragma once
#include "LogFilter.h"
#include "include\ILogResultProcessor.h"
#include <queue>
#include <string>
using namespace std;
namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogReader
		{
		public:
			LogReader() = default;
			virtual ~LogReader() = default;

			static void Read(const string& filePath, const shared_ptr<LogFilter> pLogFilter, shared_ptr<ILogResultProcessor> logResultProcessorPtr);
		};
	}
}

