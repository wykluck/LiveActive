#pragma once
#include <map>
#include <deque>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include "ILogResultProcessor.h"
using namespace std;


namespace SmartAnalyzer
{
	namespace Logging
	{
		class __declspec(dllexport) LogScanner : public ILogResultProcessor
		{
		public:
			LogScanner(const string& baseDir, const string& patternFile);
			virtual ~LogScanner();


			//virtual void Config(vector<ILogEntryProcessor>& processorVec);
			void Scan(int minResultCounts, bool needSorting);
			void Pause();
			void Stop();
			bool IsFinished();
			virtual void PushFilteredResults(deque<LogEntry>& filteredResultQueue);
			deque<LogEntry> RetrieveResults();

		private:
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
			}RegexStruct;

			string m_baseDir;
			map<string, RegexStruct> m_dirLogPatternMap;
			Status m_status;
			deque<LogEntry> m_resultQueue;
			std::mutex m_mtx;
			std::condition_variable m_cv;
			int m_minResultCounts;
		};
	}
}

