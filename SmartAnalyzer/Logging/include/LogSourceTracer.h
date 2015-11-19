#pragma once
#include <string>
#include <vector>
#include <boost/xpressive/xpressive.hpp>
using namespace std;
namespace SmartAnalyzer
{
	namespace Logging
	{

		struct TraceSourceInfo
		{
			string sourceBaseDir;
			string extensions;
			string openCommand;
			string directRegex;
			string openArgs;
		};

		class __declspec(dllexport) LogSourceTracer
		{
		public:
			LogSourceTracer(const TraceSourceInfo& traceSourceInfo);
			virtual ~LogSourceTracer();

			struct TraceSubResult
			{
				TraceSubResult(const string& filePath, int lineNumber, float matchRatio, const string& openCommand)
					: m_filePath(filePath), m_lineNumber(lineNumber), m_matchRatio(matchRatio), m_openCommand(openCommand)
				{};
				TraceSubResult(const TraceSubResult& other)
				{
					m_filePath = other.m_filePath;
					m_lineNumber = other.m_lineNumber;
					m_matchRatio = other.m_matchRatio;
					m_openArgs = other.m_openArgs;
					m_openCommand = other.m_openCommand;
				}
				string m_filePath;
				int m_lineNumber;
				float m_matchRatio;
				string m_openArgs;
				string m_openCommand;
			};
			struct TraceResult
			{
				TraceResult() : m_bSuccess(false)
				{};
			
				TraceResult(const TraceResult& other)
				{
					m_bSuccess = other.m_bSuccess;
					subResultVec.assign(other.subResultVec.begin(), other.subResultVec.end());
				}
				~TraceResult()
				{
					subResultVec.clear();
				}
				
				bool m_bSuccess;
				vector<TraceSubResult> subResultVec;		
			};
			
			TraceResult Trace(const string& logEntryLine);

		private:
			TraceResult DirectTrace(const string& logEntryLine, const boost::xpressive::smatch& what);
			TraceResult SearchTrace(const string& logEntryLine);

			TraceSourceInfo m_traceSourceInfo;

		};
	}
}