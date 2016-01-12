#pragma once
#include <string>
namespace SmartAnalyzer
{
	namespace CallGraph
	{
		struct FuncDefinition
		{
		public:
			FuncDefinition() : m_startLine(-1), m_endLine(-1), m_isVirtual(false), m_isPublic(false), m_id(-1)
			{

			}
			int m_id;
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

		struct FuncCall
		{
		public:
			FuncCall() : m_sourceId(-1), m_targetId(-1), m_startLine(-1)
			{

			}
			int m_sourceId;
			int m_targetId;
			std::string m_sourceFilePath;
			int m_startLine;
			std::string m_comments;
		};
	}
}
