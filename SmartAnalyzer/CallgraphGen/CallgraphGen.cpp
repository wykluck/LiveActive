#include "FunctionExtractor.h"

using namespace SmartAnalyzer::CallGraph;

void main(int argc, char **argv)
{
	if (argc < 3) {
		printf("\nUsage: CallgraphGen foo.bsc bar.dot functionIntials\n\n");
		printf("    e.g. CallgraphGen foo.bsc bar.dot *\n");
		exit(0);
	}

	FunctionExtractor funcExtractor(argv[1]);

	std::set<std::string> functionInitials;
	for (auto i = 3; i < argc; i++)
		functionInitials.insert(argv[i]);
	auto resQueue = funcExtractor.ExtractFuncDefs(functionInitials);

	if (!resQueue.empty())
	{
		//TODO: generate the dot file using the resQueue
	}
}
