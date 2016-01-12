#pragma once
#include <string>
#include <queue>
#include <set>
#include <map>
#include "FuncDefinition.h"
#include "bsc.h"
namespace SmartAnalyzer
{
	namespace CallGraph
	{
		struct FuncExtractResult
		{
			std::map<int, FuncDefinition> m_funcDefMap;
			std::vector<FuncCall> m_funcCallVec;
		};

		class FuncExtractor
		{
		public:
			FuncExtractor(const std::string& bscFilePath);
			virtual ~FuncExtractor();

			FuncExtractResult Extract(const std::set<std::string>& functionInitials);

		private:
			Bsc* m_pbsc;
			MBF m_mbf;
		};

	}
}