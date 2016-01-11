#pragma once
#include <string>
#include <queue>
#include <set>
#include "FuncDefinition.h"
#include "bsc.h"
namespace SmartAnalyzer
{
	namespace CallGraph
	{
		class FunctionExtractor
		{
		public:
			FunctionExtractor(const std::string& bscFilePath);
			virtual ~FunctionExtractor();

			std::queue<FuncDefinition> ExtractFuncDefs(const std::set<std::string>& functionInitials);

		private:
			Bsc* m_pbsc;
			MBF m_mbf;
		};

	}
}