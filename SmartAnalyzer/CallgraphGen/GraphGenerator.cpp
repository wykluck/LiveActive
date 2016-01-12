#include "GraphGenerator.h"
#include <utility> 
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;
using namespace SmartAnalyzer::CallGraph;

GraphGenerator::GraphGenerator()
{

}

GraphGenerator::~GraphGenerator()
{

}

bool GraphGenerator::Generate(const FuncExtractResult& funcExtractResult, const std::string& outputFilePath)
{
	// create a typedef for the Graph type
	typedef adjacency_list<vecS, vecS, directedS> Graph;

	/*Graph g(funcExtractResult.m_funcDefMap.size());

	for (auto itr = funcExtractResult.m_funcDefMap.begin(); itr != funcExtractResult.m_funcDefMap.end(); itr++)
	{
		Graph::vertex_descriptor vertex = boost::add_vertex(g);
		vertex.
	}
	*/
	return true;
}