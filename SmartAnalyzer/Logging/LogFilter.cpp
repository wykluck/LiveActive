#include "LogFilter.h"
#include <boost/tokenizer.hpp>

namespace SmartAnalyzer
{
	namespace Logging
	{


		std::shared_ptr<LogFilter> CreateJavaLogFilter(string& filterDesc, string& timeRegexStr, string& regexStr)
		{
			typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
			boost::char_separator<char> sep{ " \t" };
			tokenizer tok{ filterDesc, sep };
			//split the filterDesc into tokens 
			bool bValid = true;
			std::shared_ptr<LogFilter> pLogFilter(new LogFilter(timeRegexStr, regexStr));
			for (const auto &t : tok)
			{
				if (t == "&&" || t == "||")
				{
					if (pLogFilter->fieldNameFilterVec.empty())
					{
						bValid = false;
						break;
					}
					pLogFilter->fieldFilterOpVec.push_back(t);
					continue;
				}
				//try to match the token with different java filters
				shared_ptr<TimeLogFieldFilter>  pTimeLogFieldFilter = TimeLogFieldFilter::MatchCreate(t, timeRegexStr);
				if (pTimeLogFieldFilter.get())
				{
					pLogFilter->fieldNameFilterVec.push_back(pTimeLogFieldFilter);
					continue;
				}
				shared_ptr<Java::LevelLogFieldFilter>  pLevelLogFieldFilter = Java::LevelLogFieldFilter::MatchCreate(t);
				if (pLevelLogFieldFilter.get())
				{
					pLogFilter->fieldNameFilterVec.push_back(pLevelLogFieldFilter);
					continue;
				}
				shared_ptr<Java::MessageLogFieldFilter>  pMessageLogFieldFilter = Java::MessageLogFieldFilter::MatchCreate(t);
				if (pTimeLogFieldFilter.get())
				{
					pLogFilter->fieldNameFilterVec.push_back(pMessageLogFieldFilter);
					continue;
				}
				bValid = false;
				break;
			}
			if (pLogFilter->fieldNameFilterVec.size() != pLogFilter->fieldFilterOpVec.size() + 1)
				bValid = false;
			if (!bValid)
				pLogFilter.reset((LogFilter*)NULL);
			return pLogFilter;
		}

		std::shared_ptr<LogFilter> CreateNginxLogFilter(string& filterDesc, string& timeRegexStr, string& regexStr)
		{
			typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
			boost::char_separator<char> sep{ " \t" };
			tokenizer tok{ filterDesc, sep };
			//split the filterDesc into tokens 
			bool bValid = true;
			std::shared_ptr<LogFilter> pLogFilter(new LogFilter(timeRegexStr, regexStr));
			for (const auto &t : tok)
			{
				if (t == "&&" || t == "||")
				{
					if (pLogFilter->fieldNameFilterVec.empty())
					{
						bValid = false;
						break;
					}
					pLogFilter->fieldFilterOpVec.push_back(t);
					continue;
				}
				//try to match the token with different java filters
				shared_ptr<TimeLogFieldFilter>  pTimeLogFieldFilter = TimeLogFieldFilter::MatchCreate(t, timeRegexStr);
				if (pTimeLogFieldFilter.get())
				{
					pLogFilter->fieldNameFilterVec.push_back(pTimeLogFieldFilter);
					continue;
				}
				shared_ptr<Nginx::StatusLogFieldFilter>  pStatusLogFieldFilter = Nginx::StatusLogFieldFilter::MatchCreate(t);
				if (pStatusLogFieldFilter.get())
				{
					pLogFilter->fieldNameFilterVec.push_back(pStatusLogFieldFilter);
					continue;
				}
				shared_ptr<Nginx::RequestLogFieldFilter>  pRequestLogFieldFilter = Nginx::RequestLogFieldFilter::MatchCreate(t);
				if (pRequestLogFieldFilter.get())
				{
					pLogFilter->fieldNameFilterVec.push_back(pRequestLogFieldFilter);
					continue;
				}
				bValid = false;
				break;
			}
			if (pLogFilter->fieldNameFilterVec.size() != pLogFilter->fieldFilterOpVec.size() + 1)
				bValid = false;
			if (!bValid)
				pLogFilter.reset((LogFilter*)NULL);
			return pLogFilter;
		}

	}
}

