#include "FuncExtractor.h"
#include "GraphGenerator.h"
using namespace SmartAnalyzer::CallGraph;

void main(int argc, char **argv)
{
	if (argc < 3) {
		printf("\nUsage: CallgraphGen foo.bsc bar.dot functionIntials\n\n");
		printf("    e.g. CallgraphGen foo.bsc bar.dot *\n");
		exit(-1);
	}

	FuncExtractor funcExtractor(argv[1]);

	std::set<std::string> functionInitials;
	for (auto i = 3; i < argc; i++)
		functionInitials.insert(argv[i]);
	auto funcExtractRes = funcExtractor.Extract(functionInitials);

	GraphGenerator graphGen;
	graphGen.Generate(funcExtractRes, argv[2]);

}
