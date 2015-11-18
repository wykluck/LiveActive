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
		};

		class LogSourceTracer
		{
		public:
			LogSourceTracer(const TraceSourceInfo& traceSourceInfo);
			virtual ~LogSourceTracer();

			struct TraceSubResult
			{
				TraceSubResult(const string& filePath, int lineNumber, float matchRatio)
					: m_filePath(filePath), m_lineNumber(lineNumber), m_matchRatio(matchRatio)
				{};
				string m_filePath;
				int m_lineNumber;
				float m_matchRatio;
			};
			struct TraceResult
			{
				bool m_bSuccess;
				vector<TraceSubResult> subResultVec;		
			};
			
			TraceResult Trace(const string& logEntryLine);
			vector<string> GetOpenCommand(const TraceResult& traceResult);

		private:
			TraceResult DirectTrace(const string& logEntryLine, const boost::xpressive::smatch& what);
			TraceResult SearchTrace(const string& logEntryLine);

			TraceSourceInfo m_traceSourceInfo;

		};
	}
}