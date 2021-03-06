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
			LogEntry(const TimeStruct& timeField, const std::string& threadId, const std::string& logString, const std::string& moduleName) 
				: m_timeField(timeField), m_logString(logString), m_moduleName(moduleName), m_threadId(threadId)
			{};
			virtual ~LogEntry() {};

			LogEntry(LogEntry&& other)
				: m_timeField(other.m_timeField),
				m_logString(std::move(other.m_logString)),
				m_moduleName(other.m_moduleName),
				m_threadId(std::move(other.m_threadId))
			{};
			LogEntry& operator=(LogEntry&& other) {
				m_timeField = other.m_timeField;
				m_logString = std::move(other.m_logString);
				m_moduleName = other.m_moduleName;
				m_threadId = std::move(other.m_threadId);
				return *this;
			}
			LogEntry(const LogEntry& other)
				: m_timeField(other.m_timeField),
				m_logString(other.m_logString),
				m_moduleName(other.m_moduleName),
				m_threadId(other.m_threadId)
			{};
			LogEntry& operator=(const LogEntry& other) {
				m_timeField = other.m_timeField;
				m_logString = other.m_logString;
				m_moduleName = other.m_moduleName;
				m_threadId = other.m_threadId;
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
			const std::string& GetModuleName()
			{
				return m_moduleName;
			}
			const std::string& GetLogThreadId()
			{
				return m_threadId;
			}
		private:
			std::string m_logString;
			TimeStruct m_timeField;
			std::string m_moduleName;
			std::string m_threadId;
		};
	}
}