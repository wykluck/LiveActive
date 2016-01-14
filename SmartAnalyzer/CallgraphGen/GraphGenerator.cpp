#include "GraphGenerator.h"
#include "FuncDefinition.h"
#include <fstream>
#include <utility> 
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>

using namespace boost;
using namespace SmartAnalyzer::CallGraph;

GraphGenerator::GraphGenerator()
{

}

GraphGenerator::~GraphGenerator()
{

}

template <class NameMap, class SourcePathMap, class LineMap, class CommentsMap>
class Vertex_Writer {
public:
	Vertex_Writer(NameMap n, SourcePathMap s, LineMap l, CommentsMap c) : name(n), sourcePath(s), line(l), comments(c) {}
	template <class Vertex>
	void operator()(std::ostream &out, const Vertex& v) const {
		out << "[label=\"" << name[v] << "\", sourcePath=\"" << sourcePath[v] << "\", line="  <<
			line[v] << ", comments=\"" << comments[v] << "\"]";
	}
private:
	NameMap name;
	SourcePathMap sourcePath;
	LineMap line;
	CommentsMap comments;
};

template <class NameMap, class SourcePathMap, class LineMap, class CommentsMap>
inline Vertex_Writer<NameMap, SourcePathMap, LineMap, CommentsMap>
make_vertex_writer(NameMap n, SourcePathMap s, LineMap l, CommentsMap c) {
	return Vertex_Writer<NameMap, SourcePathMap, LineMap, CommentsMap>(n, s, l, c);
}

bool GraphGenerator::Generate(const FuncExtractResult& funcExtractResult, const std::string& outputFilePath)
{
	// create a typedef for the Graph type
	typedef adjacency_list<vecS, vecS, directedS, FuncDefinition, FuncCall> Graph;

	Graph g;
	std::map<int, Graph::vertex_descriptor> idVertexDescMap;
	// add vertices from funcExtractResult.m_funcDefMap
	for (auto itr = funcExtractResult.m_funcDefMap.begin(); itr != funcExtractResult.m_funcDefMap.end(); itr++)
	{
		Graph::vertex_descriptor vDes = boost::add_vertex(g);
		g[vDes] = itr->second;
		idVertexDescMap[itr->second.m_id] = vDes;
	}
	
	//add edges from funcExtractResult.m_funcCallVec
	for (auto itr = funcExtractResult.m_funcCallVec.begin(); itr != funcExtractResult.m_funcCallVec.end(); itr++)
	{
		Graph::vertex_descriptor u = idVertexDescMap[itr->m_sourceId];
		Graph::vertex_descriptor v = idVertexDescMap[itr->m_targetId];
		auto res = boost::add_edge(u, v, g);
		if (res.second)
			g[res.first] = *itr;
	}

	idVertexDescMap.clear();

	std::ofstream fout(outputFilePath.c_str());
	boost::write_graphviz(fout, g, make_vertex_writer(boost::get(&FuncDefinition::m_fullQualifiedName, g), 
		boost::get(&FuncDefinition::m_sourceFilePath, g), boost::get(&FuncDefinition::m_startLine, g),
		boost::get(&FuncDefinition::m_comments, g)));

	return true;
}