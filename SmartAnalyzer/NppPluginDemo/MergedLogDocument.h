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
			MergedLogDocument(const std::string& moduleName, const std::map<std::string, std::shared_ptr<Logging::LogSourceTracer>>& moduleIndexTracerMap)
				: LogDocumentBase(moduleName), m_moduleNameTracerMap(moduleIndexTracerMap)
			{

			}
			virtual ~MergedLogDocument() {};
			virtual void ImportFrom(HWND curScintilla, std::deque<Logging::LogEntry>& logResultQueue);
			virtual void Trace();
			virtual void SyncTimeWith(LogDocumentBase& other);
			virtual void Split();

		private:
			struct LineGroupModuleInfo
			{
				unsigned int startLine;
				unsigned int endLine;
				std::string moduleName;
			};

			
			std::vector<LineGroupModuleInfo> m_lineGroupModuleInfoVec;
			std::map<std::string, std::shared_ptr<Logging::LogSourceTracer>> m_moduleNameTracerMap;
		};
	}
}