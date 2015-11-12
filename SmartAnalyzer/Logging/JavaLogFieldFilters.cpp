#include "JavaLogFieldFilters.h"

using namespace SmartAnalyzer::Logging;
using namespace SmartAnalyzer::Logging::Java;

map<string, LevelLogFieldFilter::Level> LevelLogFieldFilter::s_LogLevelStrEnumMap = LevelLogFieldFilter::CreateLogLevelStrEnumMap();

map<string, LevelLogFieldFilter::Level> LevelLogFieldFilter::CreateLogLevelStrEnumMap()
{
	if (s_LogLevelStrEnumMap.size() == 0)
	{
		map<string, LevelLogFieldFilter::Level> tmpMap;
		tmpMap.insert(make_pair("TRACE", Level::TRACE));
		tmpMap.insert(make_pair("DEBUG", Level::DEBUG));
		tmpMap.insert(make_pair("INFO", Level::INFO));
		tmpMap.insert(make_pair("WARN", Level::WARN));
		tmpMap.insert(make_pair("ERROR", Level::ERROR));
		tmpMap.insert(make_pair("FATAL", Level::FATAL));

		return tmpMap;
	}
}


bool LevelLogFieldFilter::Filter(const string& strField) const
{
	auto itr = s_LogLevelStrEnumMap.find(strField);
	if (itr != s_LogLevelStrEnumMap.end())
	{
		return itr->second >= m_threshold;
	}
	return false;
}