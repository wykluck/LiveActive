#pragma once
#include "LogDocumentBase.h"
#include "LogSourceTracer.h"
#include <map>
namespace SmartAnalyzer
{
	namespace NPPLogging
	{
		class MergedLogDocument : public LogDocumentBase
		{
		public:
			MergedLogDocument(const std::string& moduleName, const std::map<unsigned short, std::shared_ptr<Logging::LogSourceTracer>>& moduleIndexTracerMap)
				: LogDocumentBase(moduleName), m_moduleIndexTracerMap(moduleIndexTracerMap)
			{

			}
			virtual ~MergedLogDocument() {};
			virtual void ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue);
			virtual void Trace();
			virtual void SyncTimeWith(LogDocumentBase& other);

		private:
			struct LineGroupModuleInfo
			{
				unsigned int startLine;
				unsigned int endLine;
				unsigned short moduleIndex;
			};

			
			std::vector<LineGroupModuleInfo> m_lineGroupModuleInfoVec;
			std::map<unsigned short, std::shared_ptr<Logging::LogSourceTracer>> m_moduleIndexTracerMap;
		};
	}
}