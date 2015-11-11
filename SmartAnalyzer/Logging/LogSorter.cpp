#include "LogSorter.h"
#include <algorithm>
using namespace SmartAnalyzer::Logging;
LogSorter::LogSorter()
{
}


LogSorter::~LogSorter()
{
}

bool compare_logentry(const LogEntry& entryi, const LogEntry& entryj)
{
	return (entryi.GetLogTime() < entryj.GetLogTime());
}

void LogSorter::PushFilteredResults(deque<LogEntry>& filteredResultQueue)
{
	//TODO: this may have very poor performance, need to address later
	std::unique_lock<std::mutex> lock(m_mtx);
	m_resultQueue.insert(m_resultQueue.end(), filteredResultQueue.begin(), filteredResultQueue.end());;
	std::stable_sort(m_resultQueue.begin(), m_resultQueue.end(), compare_logentry);
}

deque<LogEntry> LogSorter::RetrieveResults()
{
	std::unique_lock<std::mutex> lock(m_mtx);
	deque<LogEntry> resultQueue;
	m_resultQueue.swap(resultQueue);
	return resultQueue;
}