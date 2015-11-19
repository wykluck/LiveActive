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
			if (regex_search(logEntryLine, what, directRegex))
			{
				string fileFullPath;
				//get the filefullname search the sourcebasedir
				recursive_directory_iterator pathItr(m_traceSourceInfo.sourceBaseDir);
				string matchFileName = what["file"];
				while (pathItr != boost::filesystem::recursive_directory_iterator())
				{
					if (is_regular_file(pathItr->path()) &&
						(matchFileName == pathItr->path().filename().string()))
					{
						//found, fill the fields and return the traceResult
						fileFullPath = pathItr->path().string();
						int lineNo = stoi(what["line"]);
						TraceSubResult subResult(fileFullPath, lineNo, 1.0, m_traceSourceInfo.openCommand);
						// replace strings like "{XYZ}" with the result of replaceMap["XYZ"]
						sregex replaceRegex = "{" >> (s1 = +_w) >> '}';
						map<string, string> replaceMap;
						replaceMap["file"] = subResult.m_filePath;
						replaceMap["line"] = to_string(subResult.m_lineNumber);
						subResult.m_openArgs = regex_replace(m_traceSourceInfo.openArgs, replaceRegex,
							[&replaceMap](smatch const &what)
						{
							return replaceMap[what[1].str()];
						});

						traceResult.m_bSuccess = true;	
						traceResult.subResultVec.push_back(subResult);
						return traceResult;
					}
					pathItr++;
				}
			}

			//try the search trace which take long time
			return SearchTrace(logEntryLine);
		}

		
		LogSourceTracer::TraceResult LogSourceTracer::SearchTrace(const string& logEntryLine)
		{
			//TODO: need to implement it
			TraceResult tracerResult;
			return tracerResult;
		}

	}
}