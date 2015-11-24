#include "LogReader.h"
#include "LogFilter.h"
#include "LogSorter.h"
#include "include\LogEntry.h"
#include <fstream>
#include <boost/xpressive/xpressive.hpp>
#include <boost/thread/tss.hpp>
#include "include\ILogResultProcessor.h"

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
		
		if (regex_search(lineStr, what, pLogFilter->wholeRegex))
		{
			bool result = pLogFilter->Filter(what);
			
			if (result)
			{
				//extract time from the log entry
				time_t timeField = TimeLogFieldFilter::ExtractTime(what[TIME_FILTER_NAME], pLogFilter->timeRegex);
				LogEntry logEntry(timeField, lineBuffer, pLogFilter->moduleIndex);
				pThreadResQueue->push_back(logEntry);
				isPreviousLineInFilter = true;
			}
		}
		else
		{
			if (isPreviousLineInFilter)
			{
				//add the exception lines to the logentry string
				pThreadResQueue->back().GetLogString().append(lineBuffer).append("\n");
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