#pragma once
namespace SmartAnalyzer
{
	namespace Logging
	{
		class LogEntry
		{
		public:
			LogEntry(const time_t& timeField, const std::string& logString, unsigned short moduleIndex) 
				: m_timeField(timeField), m_logString(logString), m_moduleIndex(moduleIndex)
			{};
			virtual ~LogEntry() {};

			LogEntry(LogEntry&& other)
				: m_timeField(other.m_timeField),
				m_logString(std::move(other.m_logString)),
				m_moduleIndex(other.m_moduleIndex)
			{};
			LogEntry& operator=(LogEntry&& other) {
				m_timeField = other.m_timeField;
				m_logString = std::move(other.m_logString);
				m_moduleIndex = other.m_moduleIndex;
				return *this;
			}
			const time_t& GetLogTime() const
			{
				return m_timeField;
			}
			const std::string& GetLogString() const
			{
				return m_logString;
			}
			std::string& GetLogString() 
			{
				return m_logString;
			}
			const unsigned short GetModuleIndex()
			{
				return m_moduleIndex;
			}

		private:
			std::string m_logString;
			time_t m_timeField;
			unsigned short m_moduleIndex;

		};
	}
}