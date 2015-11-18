#pragma once
#include <map>
#include "JavaLogFieldFilters.h"
#include "NginxLogFieldFilters.h"
using namespace boost::xpressive;

namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogFilter
		{
		public:
			//name and value pair for the filter
			// it can be a time range "time" --> (epoch start, epoch end)
			// "message" --> message regex
			// "loglevel" --> (loglevel threshold)
			// 
			vector<shared_ptr<ILogFieldFilter>> fieldNameFilterVec;
			// (time && ( message || level )
			vector<string> fieldFilterOpVec;
			//each name should be in a regex name capture
			sregex timeRegex;
			sregex wholeRegex;
			unsigned short moduleIndex;
			LogFilter(string& timeRegexStr, string& regexStr, unsigned short moduleIndex_)
			{
				wholeRegex = sregex::compile(regexStr);
				timeRegex = sregex::compile(timeRegexStr);
				moduleIndex = moduleIndex_;
			}

			bool Filter(const smatch& what) const
			{
				//time=(2015-11-02, 2015-11-03) && message=(dfdfd, normal) && level>=(warn)
				bool res = fieldNameFilterVec[0]->Filter(what[fieldNameFilterVec[0]->GetName()]);

				for (auto i = 1; i < fieldNameFilterVec.size(); i++)
				{
					if (fieldFilterOpVec[i - 1] == "&&")
					{
						res = res && fieldNameFilterVec[i]->Filter(what[fieldNameFilterVec[i]->GetName()]);
					}
					else if (fieldFilterOpVec[i - 1] == "||")
					{
						res = res || fieldNameFilterVec[i]->Filter(what[fieldNameFilterVec[i]->GetName()]);
					}					
				}

				return res;
			}


		};

		std::shared_ptr<LogFilter> CreateJavaLogFilter(string& filterDesc, string& timeRegexStr, string& regexStr, unsigned short moduleIndex);

		std::shared_ptr<LogFilter> CreateNginxLogFilter(string& filterDesc, string& timeRegexStr, string& regexStr, unsigned short moduleIndex);

	}
}