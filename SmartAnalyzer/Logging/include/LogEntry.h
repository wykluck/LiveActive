#pragma once
namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogEntry
		{
		public:
			LogEntry(const time_t& timeField, const std::string& logString) : m_timeField(timeField), m_logString(logString)
			{};
			virtual ~LogEntry() {};


			const time_t& GetLogTime() const
			{
				return m_timeField;
			}
			const std::string& GetLogString() const
			{
				return m_logString;
			}

		private:
			std::string m_logString;
			time_t m_timeField;

		};
	}
}