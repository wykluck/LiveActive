#include "LogFilter.h"
namespace SmartAnalyzer
{
	namespace Logging
	{

		std::shared_ptr<LogFilter> CreateTestJavaLogFilter(string& timeRegexStr, string& regexStr)
		{
			std::shared_ptr<LogFilter> pLogFilter(new LogFilter(timeRegexStr, regexStr));
			tm tm_from, tm_to;
			tm_from.tm_year = 2015 - 1900;
			tm_from.tm_mon = 10;
			tm_from.tm_mday = 1;
			tm_from.tm_hour = 0;
			tm_from.tm_min = 0;
			tm_from.tm_sec = 0;
			tm_to.tm_year = 2015 - 1900;
			tm_to.tm_mon = 10;
			tm_to.tm_mday = 30;
			tm_to.tm_hour = 23;
			tm_to.tm_min = 59;
			tm_to.tm_sec = 59;
			time_t from = mktime(&tm_from);
			time_t to = mktime(&tm_to);

			shared_ptr<TimeLogFieldFilter> pTimeLogFieldFilter(new TimeLogFieldFilter(from, to, pLogFilter->timeRegex));
			pLogFilter->fieldNameFilterVec.push_back(pTimeLogFieldFilter);
			pLogFilter->fieldFilterOpVec.push_back("&&");
			shared_ptr<Java::LevelLogFieldFilter> pLevelLogFieldFilter(new Java::LevelLogFieldFilter("WARN"));
			pLogFilter->fieldNameFilterVec.push_back(pLevelLogFieldFilter);
			//shared_ptr<Java::MessageLogFieldFilter> pMsgLogFieldFilter(new Java::MessageLogFieldFilter("error"));
			//pLogFilter->fieldNameFilterVec.push_back(pMsgLogFieldFilter);
			


			return pLogFilter;
		}

		std::shared_ptr<LogFilter> CreateTestNginxLogFilter(string& timeRegexStr, string& regexStr)
		{
			std::shared_ptr<LogFilter> pLogFilter(new LogFilter(timeRegexStr, regexStr));
			tm tm_from, tm_to;
			tm_from.tm_year = 2015 - 1900;
			tm_from.tm_mon = 10;
			tm_from.tm_mday = 1;
			tm_from.tm_hour = 0;
			tm_from.tm_min = 0;
			tm_from.tm_sec = 0;
			tm_to.tm_year = 2015 - 1900;
			tm_to.tm_mon = 10;
			tm_to.tm_mday = 30;
			tm_to.tm_hour = 23;
			tm_to.tm_min = 59;
			tm_to.tm_sec = 59;
			time_t from = mktime(&tm_from);
			time_t to = mktime(&tm_to);

			shared_ptr<TimeLogFieldFilter> pTimeLogFieldFilter(new TimeLogFieldFilter(from, to, pLogFilter->timeRegex));
			pLogFilter->fieldNameFilterVec.push_back(pTimeLogFieldFilter);
			pLogFilter->fieldFilterOpVec.push_back("&&");
			std::set<int> statusCodeSet = { 400, 500, 401, 503, 504 };
			shared_ptr<Nginx::StatusLogFieldFilter> pMsgLogFieldFilter(new Nginx::StatusLogFieldFilter(statusCodeSet));
			pLogFilter->fieldNameFilterVec.push_back(pMsgLogFieldFilter);

			return pLogFilter;
		}
	}
}

