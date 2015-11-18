#include "include\LogSourceTracer.h"
#include <boost/xpressive/xpressive.hpp>
#include <boost/filesystem.hpp>

using namespace boost::xpressive;
using namespace boost::filesystem;

namespace SmartAnalyzer
{
	namespace Logging
	{
		LogSourceTracer::LogSourceTracer(const TraceSourceInfo& traceSourceInfo)
			: m_traceSourceInfo(traceSourceInfo)
		{

		}
		
		LogSourceTracer::~LogSourceTracer()
		{

		}

		LogSourceTracer::TraceResult LogSourceTracer::Trace(const string& logEntryLine)
		{
			//try the direct trace first
			sregex directRegex = sregex::compile(m_traceSourceInfo.directRegex);
			smatch what;
			TraceResult traceResult;
			traceResult.m_bSuccess = false;
			if (regex_search(logEntryLine, what, directRegex))
			{
				string fileFullPath;
				//get the filefullname search the sourcebasedir
				recursive_directory_iterator pathItr(m_traceSourceInfo.sourceBaseDir);
				while (pathItr != boost::filesystem::recursive_directory_iterator())
				{
					if (is_regular_file(pathItr->path()) ||
						(pathItr->path().filename().string() == what["file"].str()))
					{
						//found, fill the fields and return the traceResult
						fileFullPath = pathItr->path().filename().string();
						int lineNo = stoi(what["line"]);
						TraceSubResult subResult(fileFullPath, lineNo, 1.0);
						traceResult.m_bSuccess = true;	
						traceResult.subResultVec.push_back(subResult);
						return traceResult;
					}
				}
			}

			//try the search trace which take long time
			return SearchTrace(logEntryLine);
		}

		vector<string> LogSourceTracer::GetOpenCommand(const LogSourceTracer::TraceResult& traceResult)
		{
			vector<string> opencommandVec;
			if (traceResult.m_bSuccess)
			{
				for (auto subResult : traceResult.subResultVec)
				{
					// replace strings like "{XYZ}" with the result of replaceMap["XYZ"]
					sregex replaceRegex = "{" >> (s1 = +_w) >> '}';
					map<string, string> replaceMap;
					replaceMap["file"] = subResult.m_filePath;
					replaceMap["line"] = to_string(subResult.m_lineNumber);
					std::string outputOpenCommand = regex_replace(m_traceSourceInfo.openCommand, replaceRegex, 
						[&replaceMap](smatch const &what)
					{	
						return replaceMap[what[1].str()];
					});	
					opencommandVec.push_back(outputOpenCommand);
				}
			}

			return opencommandVec;
		}
		
		LogSourceTracer::TraceResult LogSourceTracer::SearchTrace(const string& logEntryLine)
		{
			//TODO: need to implement it
		}

	}
}