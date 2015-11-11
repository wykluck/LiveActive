#pragma once
#include <string>
#include <boost/xpressive/xpressive.hpp>
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
		};

		class TimeLogFieldFilter : public ILogFieldFilter
		{
		public:
			TimeLogFieldFilter(const time_t& from, const time_t& to, const sregex& timeRegex)
				: m_from(from), m_to(to), m_timeRegex(timeRegex)
			{};
			virtual ~TimeLogFieldFilter() {};
			virtual bool Filter(const string& strField) const;
			static time_t ExtractTime(const string& timeStrField, const sregex& timeRegex);
			
			virtual string GetName() const
			{
				return TIME_FILTER_NAME;
			};
		private:
			time_t m_from;
			time_t m_to;
			sregex m_timeRegex;
			static std::map<std::string, int> s_MonthStrToIntMap;
			static map<std::string, int> CreateMonthStrToIntMap();
		};

		

	}
}
