#include "ILogFieldFilter.h"

namespace SmartAnalyzer
{
	namespace Logging
	{

		std::map<std::string, int> TimeLogFieldFilter::s_MonthStrToIntMap = TimeLogFieldFilter::CreateMonthStrToIntMap();
		map<std::string, int> TimeLogFieldFilter::CreateMonthStrToIntMap()
		{
			map<std::string, int> tmpMap;
			tmpMap.insert(make_pair("Jan", 1));
			tmpMap.insert(make_pair("Feb", 2));
			tmpMap.insert(make_pair("Mar", 3));
			tmpMap.insert(make_pair("Apr", 4));
			tmpMap.insert(make_pair("May", 5));
			tmpMap.insert(make_pair("June", 6));
			tmpMap.insert(make_pair("July", 7));
			tmpMap.insert(make_pair("Aug", 8));
			tmpMap.insert(make_pair("Sept", 9));
			tmpMap.insert(make_pair("Oct", 10));
			tmpMap.insert(make_pair("Nov", 11));
			tmpMap.insert(make_pair("Dec", 12));

			return tmpMap;
		}

		bool TimeLogFieldFilter::Filter(const string& strField) const
		{
			time_t timeField = ExtractTime(strField, m_timeRegex);
			return (timeField >= m_from && timeField <= m_to);
		}

		time_t TimeLogFieldFilter::ExtractTime(const string& timeStrField, const sregex& timeRegex)
		{
			//Parse strField against its time regex to tm
			smatch what;
			tm tmField;
			if (regex_search(timeStrField, what, timeRegex))
			{
				tmField.tm_year = stoi(what["year"]) - 1900;
				try
				{
					tmField.tm_mon = stoi(what["month"]) - 1;
				}
				catch (std::invalid_argument& ex)
				{
					auto itr = s_MonthStrToIntMap.find(what["month"]);
					if (itr == s_MonthStrToIntMap.end())
						throw ex;
					tmField.tm_mon = itr->second - 1;
				}
				tmField.tm_mday = stoi(what["day"]);
				tmField.tm_hour = stoi(what["hour"]);
				tmField.tm_min = stoi(what["min"]);
				tmField.tm_sec = stoi(what["sec"]);
			}

			time_t timeField = mktime(&tmField);
			return timeField;
		}
	}
}