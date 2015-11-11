#include "include\LogScanner.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
#include <boost/threadpool.hpp>
#include "LogReader.h"
#include "LogFilter.h"
#include "LogSorter.h"
#include "json/json.h"

using namespace boost::threadpool;
using namespace boost::filesystem;
using namespace SmartAnalyzer::Logging;
const int THREAD_POOL_COUNT = 6;


LogScanner::LogScanner(const string& baseDir, const string& patternFile)
{
	m_baseDir = baseDir;
	string patternFilePath = baseDir;
	patternFilePath.append("\\").append(patternFile);
	ifstream patternFileStream(patternFilePath);
	Json::Value jsonRootValue;
	patternFileStream >> jsonRootValue;
	//open the pattern json file and load the content m_dirLogPatternMap
	if (jsonRootValue.isArray())
	{
		Json::ValueIterator jsonValueItr = jsonRootValue.begin();
		while (jsonValueItr != jsonRootValue.end())
		{
			string dirStr = (*jsonValueItr)["pathpattern"].asString();
			RegexStruct regexStruct;
			regexStruct.timeRegexStr = (*jsonValueItr)["timeregex"].asString();
			regexStruct.wholeRegexStr = (*jsonValueItr)["wholeregex"].asString();
			regexStruct.type = (*jsonValueItr)["type"].asString();
			m_dirLogPatternMap.insert(make_pair(dirStr, regexStruct));
			jsonValueItr++;
		}
	}
	patternFileStream.close();

	m_status = NotConfigured;
}
	
LogScanner::~LogScanner()
{

}

// You could also take an existing vector as a parameter.
vector<string> split(const string& str, char delimiter) {
	vector<string> internal;
	stringstream ss(str); // Turn the string into a stream.
	string tok;

	while (getline(ss, tok, delimiter)) {
		internal.push_back(tok);
	}

	return internal;
}

bool path_contains_file(const string& patternPath, const string& filePath)
{
	//split the pattern path using "\"
	vector<string> patternPathVec = split(patternPath, '\\');
	vector<string> filePathVec = split(filePath, '\\');
	for (int i = 0; i < patternPathVec.size(); i++)
	{
		if (i >= filePathVec.size())
			return false;
		if (patternPathVec[i] == "*")
		{
			//pure "*", ignore the section and make it match
			continue;
		}
		else 
		{
			//only accept end with "*"
			std::size_t startStarPos = patternPathVec[i].find("*");
			
			if (startStarPos == patternPathVec[i].npos)
			{
				//no "*", just compare two normal strings	
				if (filePathVec[i] != patternPathVec[i])
					return false;
			}
			else
			{	
				if (filePathVec[i].find(patternPathVec[i].substr(0, startStarPos)) == filePath.npos)
					return false;
			}
		}
	}
	return true;
}

	
void LogScanner::Scan(int minResultCounts, bool needSorting)
{
	m_minResultCounts = minResultCounts;
	recursive_directory_iterator pathItr(m_baseDir);
	pool threadPool(THREAD_POOL_COUNT);
	//initialise a dir to logfilter map from m_dirLogPatternMap
	std::map<string, shared_ptr<LogFilter>> dirLogFilterMap;
	for (auto itr = m_dirLogPatternMap.begin(); itr != m_dirLogPatternMap.end(); itr++)
	{
		if (itr->second.type == "java")
		{
			dirLogFilterMap.insert(make_pair(itr->first, CreateTestJavaLogFilter(itr->second.timeRegexStr, itr->second.wholeRegexStr)));
		}
		else if (itr->second.type == "nginx")
		{
			dirLogFilterMap.insert(make_pair(itr->first, CreateTestNginxLogFilter(itr->second.timeRegexStr, itr->second.wholeRegexStr)));
		}
	}
	ILogResultProcessor *pResultProcessor = NULL;
	if (needSorting)
	{
		//create a LogSorter to sort the results if needed
		pResultProcessor = new LogSorter();
	}
	else
		pResultProcessor = this;
	while (pathItr != boost::filesystem::recursive_directory_iterator())
	{
		if (is_regular_file(pathItr->path()))
		{
			//schedule a LogReader to read log entries from the file
			for (auto itr = dirLogFilterMap.begin(); itr != dirLogFilterMap.end(); itr++)
			{
				if (path_contains_file(itr->first, pathItr->path().string()))
				{
					
					auto logRead = std::bind(&LogReader::Read, pathItr->path().string(), itr->second, pResultProcessor);

					threadPool.schedule(logRead);
					//do not go to another mapping
					break;
				}
			}	
			
		}
		++pathItr;
		
	}

	threadPool.wait();
	m_status = Finished;
	if (needSorting)
	{
		m_resultQueue.swap(pResultProcessor->RetrieveResults());
		delete pResultProcessor;
	}
	m_cv.notify_one();
}
	
void LogScanner::Pause()
{

}

void LogScanner::Stop()
{

}

void LogScanner::PushFilteredResults(deque<LogEntry>& filteredResultQueue)
{
	std::lock_guard<std::mutex> lock(m_mtx);
	m_resultQueue.insert(m_resultQueue.end(), filteredResultQueue.begin(), filteredResultQueue.end()); 
	if (m_resultQueue.size() >= m_minResultCounts)
	{
		//if the result counts is bigger than minResultCounts, 
		//notify the waiting thread and wait for result retrieval
		m_cv.notify_one();

	}
}

deque<LogEntry> LogScanner::RetrieveResults()
{
	if (m_status == Finished)
	{
		//Scanning is finished, just return the whole result queue
		deque<LogEntry> resultQueue;
		m_resultQueue.swap(resultQueue);
		return resultQueue;
	}
	else
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		m_cv.wait(lock);
		deque<LogEntry> resultQueue;
		m_resultQueue.swap(resultQueue);
		return resultQueue;
	}
}

bool LogScanner::IsFinished()
{
	return (m_status == Finished);
}