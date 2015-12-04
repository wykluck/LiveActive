#pragma once
#include <map>
#include <deque>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <memory>
#include "ILogResultProcessor.h"
#include "LogSourceTracer.h"
using namespace std;


namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogFilter;
		class __declspec(dllexport) LogScanner
		{
		public:
			LogScanner(const string& baseDir, const string& patternFilePath);
			virtual ~LogScanner();

			const map<std::string, shared_ptr<LogSourceTracer>>& GetModuleNameTracerMap();


			bool Scan(const string& filterFilePath, bool bSplitInModule);
			void Pause();
			void Stop();
			bool IsFinished();
			map<string, deque<LogEntry>> RetrieveResults();

		private:
			bool LogScanner::ParseFilter(const string& filterFilePath);
			typedef enum {
				NotConfigured,
				Configured,
				Started,
				Paused,
				Stopped,
				Finished
			}Status;

			typedef struct {
				string timeRegexStr;
				string wholeRegexStr;
				string type;
				string dirPath;
				unsigned short moduleIndex;
			}RegexStruct;


			string m_baseDir;
			map<string, RegexStruct> m_moduleLogPatternMap;
			map<string, shared_ptr<LogFilter>> m_dirLogFilterMap;
			map<string, shared_ptr<LogSourceTracer>> m_moduleNameTracerMap;
			Status m_status;
			map<string, shared_ptr<ILogResultProcessor>> m_moduleResultProcessorMap;
			std::mutex m_mtx;
			std::condition_variable m_cv;
		};
	}
}

