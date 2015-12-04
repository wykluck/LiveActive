#pragma once
#include "TimeStruct.h"
namespace SmartAnalyzer
{
	namespace Logging
	{
		static const int MAX_LINE_CHARACTERS = 1024;
		class LogEntry
		{
		public:
			LogEntry(const TimeStruct& timeField, const std::string& logString, unsigned short moduleIndex) 
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
			const TimeStruct& GetLogTime() const
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
			const std::string& GetLogThreadId()
			{
				return m_threadId;
			}
		private:
			std::string m_logString;
			TimeStruct m_timeField;
			unsigned short m_moduleIndex;
			std::string m_threadId;
		};
	}
}