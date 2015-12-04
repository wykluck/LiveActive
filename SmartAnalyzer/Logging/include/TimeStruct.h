#pragma once
#include <type_traits>
#include <ctime>
/*
 * The time resolution has been assigned to milliseconds, but it is not hard to change
 */
namespace SmartAnalyzer
{
	namespace Logging
	{
		struct TimeStruct64
		{
			TimeStruct64() = default;
			TimeStruct64(tm& datetime, unsigned short millisecond)
			{
				time_t tmpTime = mktime(&datetime);
				m_time = tmpTime * 1000 + millisecond;
			}
			bool operator<(const TimeStruct64& other) const
			{
				return m_time < other.m_time;
			}
			bool operator==(const TimeStruct64& other) const
			{
				return m_time == other.m_time;
			}
			bool operator<=(const TimeStruct64& other) const
			{
				return (*this < other) || (*this == other);
			}
			bool operator>=(const TimeStruct64& other) const
			{
				return !(*this < other);
			}
			bool operator!=(const TimeStruct64& other) const
			{
				return !(*this == other);
			}
			void Set(const time_t& datetime, unsigned short millisecond)
			{
				m_time = datetime * 1000 + millisecond;
			}
			time_t m_time;
		};

		struct TimeStruct32
		{
			TimeStruct32() = default;
			TimeStruct32(tm& datetime, unsigned short millisecond)
			{
				time_t timeField = mktime(&datetime);
				m_millisconds = millisecond;
			}
			bool operator<(const TimeStruct32& other) const
			{
				if (m_time != other.m_time)
				{
					return (m_time < other.m_time);
				}
				else
					return m_millisconds < other.m_millisconds;
			}
			bool operator==(const TimeStruct32& other) const
			{
				return (m_time == other.m_time && m_millisconds == other.m_millisconds);
			}
			bool operator<=(const TimeStruct32& other) const
			{
				return (*this < other) || (*this == other);
			}
			bool operator>=(const TimeStruct32& other) const
			{
				return !(*this < other);
			}
			bool operator!=(const TimeStruct32& other) const
			{
				return !(*this == other);
			}
			void Set(const time_t& datetime, unsigned short millisecond)
			{
				m_time = datetime;
				m_millisconds = millisecond;
			}
			time_t m_time;
			unsigned short m_millisconds;
		};

		typedef std::conditional<sizeof(time_t) == 8, TimeStruct64, TimeStruct32>::type TimeStruct;
	}
}