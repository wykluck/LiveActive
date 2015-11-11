#include "ILogFieldFilter.h"
#include <boost/xpressive/xpressive.hpp>
using namespace std;
using namespace boost::xpressive;


namespace SmartAnalyzer
{
	namespace Logging
	{
		namespace Java
		{

			const std::string LEVEL_FILTER_NAME = "level";
			const std::string MESSAGE_FILTER_NAME = "message";

			class MessageLogFieldFilter : public ILogFieldFilter
			{
			public:
				MessageLogFieldFilter(const string& normalMsg)
				{
					m_normalMsg = normalMsg;
				}
				//MessageLogFieldFilter(const sregex& sRegex) : m_sRegex(sRegex)
				//{};
				virtual ~MessageLogFieldFilter() {};
				virtual bool Filter(const string& strField) const
				{
					//match the regex to strField
					//return (regex_match(strField, m_sRegex));
					return (strField.find(m_normalMsg) != strField.npos);
				};
				virtual string GetName() const
				{
					return MESSAGE_FILTER_NAME;
				};
			private:
				//sregex m_sRegex;
				string m_normalMsg;
			};
			
			class LevelLogFieldFilter : public ILogFieldFilter
			{
			public:
				enum class Level
				{
					TRACE = 0,
					DEBUG = 1,
					INFO = 2,
					WARN = 3,
					ERROR = 4,
					FATAL = 5
				};

				LevelLogFieldFilter(const string& thresholdStr)
				{
					auto itr = s_LogLevelStrEnumMap.find(thresholdStr);
					if (itr != s_LogLevelStrEnumMap.end())
					{
						m_threshold = itr->second;
					}
				}
				virtual ~LevelLogFieldFilter() {};
				virtual bool Filter(const string& strField) const;
				virtual string GetName() const
				{
					return LEVEL_FILTER_NAME;
				};

			private:
				static map<string, LevelLogFieldFilter::Level> LevelLogFieldFilter::CreateLogLevelStrEnumMap();
				static map<string, Level> s_LogLevelStrEnumMap;
				Level m_threshold;
			};
		}
	}
}