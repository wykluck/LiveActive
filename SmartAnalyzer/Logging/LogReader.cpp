#include "LogReader.h"
#include "LogFilter.h"
#include "LogSorter.h"
#include "include/LogEntry.h"
#include <fstream>
#include <boost/xpressive/xpressive.hpp>
#include <boost/thread/tss.hpp>
#include "include/ILogResultProcessor.h"

using namespace boost::xpressive;
using namespace SmartAnalyzer::Logging;

static boost::thread_specific_ptr< deque<LogEntry> > pThreadResQueue;

void LogReader::Read(const string& filePath, const shared_ptr<LogFilter> pLogFilter, shared_ptr<ILogResultProcessor> logResultProcessorPtr)
{
	if (!pThreadResQueue.get()) {
		// first time called by this thread
		// construct test element to be used in all subsequent calls from this thread
		pThreadResQueue.reset(new deque<LogEntry>());
	}
	//open the file using a filestream
	ifstream fileStream(filePath, ifstream::in);
	char lineBuffer[MAX_LINE_CHARACTERS];
	smatch what;
	bool isPreviousLineInFilter = false;
	string exceptionStr = "";
	while (fileStream.good())
	{
		fileStream.getline(lineBuffer, MAX_LINE_CHARACTERS);

		//match the lineBuffer with a regex specified by a filter
		string lineStr(lineBuffer);
		
		sregex wholeRegex = sregex::compile(pLogFilter->wholeRegexStr);
		if (regex_search(lineStr, what, wholeRegex))
		{
			bool result = pLogFilter->Filter(what);
			
			if (result)
			{
				//extract time from the log entry
				TimeStruct timeField = TimeLogFieldFilter::ExtractTime(what[TIME_FILTER_NAME], pLogFilter->timeRegexStr);
				//extract thread from the log entry if presented
				string threadId = "";
				if (pLogFilter->wholeRegexStr.find("?P<thread>") != pLogFilter->wholeRegexStr.npos)
					threadId = what[Java::THREAD_FILTER_NAME];
				LogEntry logEntry(timeField, threadId, lineBuffer, pLogFilter->moduleName);
				pThreadResQueue->push_back(logEntry);
				isPreviousLineInFilter = true;
			}
		}
		else
		{
			if (isPreviousLineInFilter)
			{
				//add the exception lines to the logentry string
				pThreadResQueue->back().GetLogString().append("\n").append(lineBuffer);
			}
		}
	}

	fileStream.close();

	//call back to push filtered results 
	if (pThreadResQueue->size() > 0)
	{
		logResultProcessorPtr->PushFilteredResults(*pThreadResQueue.get());
		//empty the queue for reuse
		pThreadResQueue->clear();
	}
}