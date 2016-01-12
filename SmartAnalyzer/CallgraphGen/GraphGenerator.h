#pragma once
#include "FuncExtractor.h"
namespace SmartAnalyzer
{
	namespace CallGraph
	{
		class GraphGenerator
		{
		public:
			GraphGenerator();
			virtual ~GraphGenerator();

			bool Generate(const FuncExtractResult& funcExtraResult, const std::string& outputFilePath);
		};
	}
}
