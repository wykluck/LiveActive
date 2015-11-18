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


LogScanner::LogScanner(const string& baseDir, const string& patternFilePath)
{
	m_baseDir = baseDir;
	ifstream patternFileStream(patternFilePath);
	Json::Value jsonRootValue;
	patternFileStream >> jsonRootValue;
	//open the pattern json file and load the content m_dirLogPatternMap
	if (jsonRootValue.isArray())
	{
		Json::ValueIterator jsonValueItr = jsonRootValue.begin();
		unsigned short moduleIndex = 1;
		while (jsonValueItr != jsonRootValue.end())
		{
			string moduleStr = (*jsonValueItr)["module"].asString();
			RegexStruct regexStruct;
			regexStruct.timeRegexStr = (*jsonValueItr)["timeregex"].asString();
			regexStruct.wholeRegexStr = (*jsonValueItr)["wholeregex"].asString();
			regexStruct.type = (*jsonValueItr)["type"].asString();
			regexStruct.dirPath = (*jsonValueItr)["pathpattern"].asString();
			regexStruct.moduleIndex = moduleIndex++;
			m_moduleLogPatternMap.insert(make_pair(moduleStr, regexStruct));

			//TODO: need to construct m_moduleIndexTracerMap

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

bool LogScanner::ParseFilter(const string& filterFilePath)
{
	ifstream patternFileStream(filterFilePath);
	Json::Value jsonRootValue;
	patternFileStream >> jsonRootValue;
	//open the pattern json file and load the content m_dirLogPatternMap
	if (jsonRootValue.isArray())
	{
		Json::ValueIterator jsonValueItr = jsonRootValue.begin();
		while (jsonValueItr != jsonRootValue.end())
		{
			string moduleStr = (*jsonValueItr)["module"].asString();
			if (moduleStr == "all")
			{
				//all module case
				string typeStr = (*jsonValueItr)["type"].asString();
				for (auto itr = m_moduleLogPatternMap.begin(); itr != m_moduleLogPatternMap.end(); itr++)
				{
					//initialise a m_dirLogFilterMap from m_moduleLogPatternMap
					if (itr->second.type == "java" && typeStr == itr->second.type)
					{
						auto pJavaLogFilter = CreateJavaLogFilter((*jsonValueItr)["filter"].asString(), itr->second.timeRegexStr, itr->second.wholeRegexStr, itr->second.moduleIndex);
						if (pJavaLogFilter.get() == NULL)
							return false;
						m_dirLogFilterMap.insert(make_pair(itr->second.dirPath, pJavaLogFilter));
					}
					else if (itr->second.type == "nginx" && typeStr == itr->second.type)
					{
						auto pNginxLogFilter = CreateNginxLogFilter((*jsonValueItr)["filter"].asString(), itr->second.timeRegexStr, itr->second.wholeRegexStr, itr->second.moduleIndex);
						if (pNginxLogFilter.get() == NULL)
							return false;
						m_dirLogFilterMap.insert(make_pair(itr->second.dirPath, pNginxLogFilter));				
					}
				}
			}
			else
			{
				//specific module case
				auto itr = m_moduleLogPatternMap.find(moduleStr);
				if (itr == m_moduleLogPatternMap.end())
					continue;
				//initialise a m_dirLogFilterMap from m_moduleLogPatternMap
				if (itr->second.type == "java")
				{
					auto pJavaLogFilter = CreateJavaLogFilter((*jsonValueItr)["filter"].asString(), itr->second.timeRegexStr, itr->second.wholeRegexStr, itr->second.moduleIndex);
					if (pJavaLogFilter.get() == NULL)
						return false;
					m_dirLogFilterMap.insert(make_pair(itr->second.dirPath, pJavaLogFilter));
				}
				else if (itr->second.type == "nginx")
				{
					auto pNginxLogFilter = CreateNginxLogFilter((*jsonValueItr)["filter"].asString(), itr->second.timeRegexStr, itr->second.wholeRegexStr, itr->second.moduleIndex);
					if (pNginxLogFilter.get() == NULL)
						return false;
					m_dirLogFilterMap.insert(make_pair(itr->second.dirPath, pNginxLogFilter));
				}
			}
			

			jsonValueItr++;
		}
	}

	return true;
}
	
bool LogScanner::Scan(const string& filterFilePath, int minResultCounts, bool needSorting)
{
	//Try to parse filter and construct m_dirLogFilterMap
	if (!ParseFilter(filterFilePath))
	{		
		m_status = Finished;
		m_cv.notify_one();
		return false;
	}
	
	m_minResultCounts = minResultCounts;
	recursive_directory_iterator pathItr(m_baseDir);
	pool threadPool(THREAD_POOL_COUNT);

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
			for (auto itr = m_dirLogFilterMap.begin(); itr != m_dirLogFilterMap.end(); itr++)
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