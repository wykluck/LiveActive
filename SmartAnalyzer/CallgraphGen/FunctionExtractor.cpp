#include "FunctionExtractor.h"
#include <boost/xpressive/xpressive.hpp>

using namespace SmartAnalyzer::CallGraph;
using namespace std;
using namespace boost::xpressive;

FunctionExtractor::FunctionExtractor(const std::string& bscFilePath)
	:m_mbf(mbfFuncs | mbfTypes | mbfClass), m_pbsc(NULL)
{
	if (!openBrowser(bscFilePath.c_str(), &m_pbsc)) {
		printf("couldn't open '%s'\n", bscFilePath.c_str());
		exit(1);
	}
}


FunctionExtractor::~FunctionExtractor()
{
	if (m_pbsc)
		m_pbsc->close();
}


std::queue<FuncDefinition> FunctionExtractor::ExtractFuncDefs(const std::set<std::string>& functionInitials)
{
	ULONG cInst;
	IINST *rgInst = NULL;
	if (!m_pbsc->getOverloadArray("*", m_mbf, &rgInst, &cInst) || cInst == 0) {
		printf("no symbols matching '%s'\n", "*");
	}
	
	std::queue<FuncDefinition> funcDefQueue;
	for (auto i = 0; i < cInst; i++)
	{
		SZ sz; TYP typ; ATR atr;
		m_pbsc->iinstInfo(rgInst[i], &sz, &typ, &atr);
		string fullFuncWithParams = m_pbsc->formatDname(sz);
		//see whether the function initial with in the set
		if (!functionInitials.empty())
		{
			auto itr = functionInitials.begin();
			for (; itr != functionInitials.end(); itr++)
			{
				if (fullFuncWithParams.find(*itr) == 0)
				{
					break;
				}
			}
			if (itr == functionInitials.end())
				continue;
		}

		std::string  type = m_pbsc->szFrTyp(typ);
		if (type == "function" || type == "mem_func")
		{
			FuncDefinition funcDef;

			sregex funcRegex = sregex::compile("(?P<func>.*)\\((?P<params>.*)\\)");
			smatch what;
			if (regex_search(fullFuncWithParams, what, funcRegex))
			{
				funcDef.m_fullQualifiedName = what["func"];
				funcDef.m_parameters = what["params"];

				IDEF *rgidef;
				ULONG cidef;
				if (!m_pbsc->getDefArray(rgInst[i], &rgidef, &cidef) || cidef == 0) {
				}
				else {
					for (auto j = 0; j < cidef; j++) {
						SZ sz; LINE line;
						m_pbsc->idefInfo(rgidef[j], &sz, &line);
						funcDef.m_sourceFilePath = sz;
						funcDef.m_startLine = line;
					}
					m_pbsc->disposeArray(rgidef);
				}
				funcDefQueue.push(funcDef);
			}

		}
	}
	
	
	return funcDefQueue;
}