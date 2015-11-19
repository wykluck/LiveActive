#pragma once
#include <string>
#include <boost/xpressive/xpressive.hpp>
#include <ctime>
namespace SmartAnalyzer
{
	namespace Logging
	{
		const std::string TIME_FILTER_NAME = "time";
		using namespace std;
		using namespace boost::xpressive;

		 

		class ILogFieldFilter
		{
		public:
			virtual bool Filter(const std::string& strField) const = 0;
			virtual std::string GetName() const = 0;
			virtual const sregex& GetFilterRegex() const
			{
				return m_filterRegex;
			}
		protected:
			//regex to capture related filter expression
			sregex m_filterRegex;
		};

		class TimeLogFieldFilter : public ILogFieldFilter
		{
		public:
			static shared_ptr<TimeLogFieldFilter> MatchCreate(const string& token, const string& timePatternRegexStr)
			{
				sregex filterRegex = sregex::compile("time=\\[(?P<from>\\d{4}-\\d{2}-\\d{2}),(?P<to>(\\d{4}-\\d{2}-\\d{2}|now))\\]");
				sregex timeRegex = sregex::compile("(?P<year>\\d{4})-(?P<month>\\d{2})-(?P<day>\\d{2})");
				smatch what;
				shared_ptr<TimeLogFieldFilter> pTimeLogFieldFilter(NULL);
				if (regex_search(token, what, filterRegex))
				{
					time_t fromTime = ExtractTime(what["from"], timeRegex, false);
					time_t toTime;
					if (what["to"] == "now")
					{
						time(&toTime);
					}
					else
					{
						toTime = ExtractTime(what["to"], timeRegex, false);
					}
					sregex timePatternRegex = sregex::compile(timePatternRegexStr);
					pTimeLogFieldFilter.reset(new TimeLogFieldFilter(fromTime, toTime, timePatternRegex));
				}
				
				return pTimeLogFieldFilter;
			}
			TimeLogFieldFilter(const time_t& from, const time_t& to, const sregex& timePatternRegex)
				: m_from(from), m_to(to), m_timePatternRegex(timePatternRegex)
			{
				
				
			};
			virtual ~TimeLogFieldFilter() {};
			virtual bool Filter(const string& strField) const;
			static time_t ExtractTime(const string& timeStrField, const sregex& timeRegex, bool bContainTime = true);
			
			virtual string GetName() const
			{
				return TIME_FILTER_NAME;
			};
			
		private:
			time_t m_from;
			time_t m_to;
			sregex m_timePatternRegex;
			static std::map<std::string, int> s_MonthStrToIntMap;
			static map<std::string, int> CreateMonthStrToIntMap();
		};

		

	}
}
