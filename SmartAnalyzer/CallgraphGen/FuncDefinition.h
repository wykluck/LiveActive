#pragma once
#include <string>
namespace SmartAnalyzer
{
	namespace CallGraph
	{
		struct FuncDefinition
		{
		public:
			FuncDefinition() : m_startLine(-1), m_endLine(-1), m_isVirtual(false), m_isPublic(false)
			{

			}
			std::string m_name;
			std::string m_fullQualifiedName;
			std::string m_parameters;
			std::string m_sourceFilePath;
			int m_startLine;
			int m_endLine;
			std::string m_comments;
			bool m_isVirtual;
			bool m_isPublic;
		};
	}
}
