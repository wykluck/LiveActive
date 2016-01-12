#include "FuncExtractor.h"
#include <boost/xpressive/xpressive.hpp>

using namespace SmartAnalyzer::CallGraph;
using namespace std;
using namespace boost::xpressive;

FuncExtractor::FuncExtractor(const std::string& bscFilePath)
	:m_mbf(mbfFuncs | mbfTypes | mbfClass), m_pbsc(NULL)
{
	if (!openBrowser(bscFilePath.c_str(), &m_pbsc)) {
		printf("couldn't open '%s'\n", bscFilePath.c_str());
		exit(1);
	}
}


FuncExtractor::~FuncExtractor()
{
	if (m_pbsc)
		m_pbsc->close();
}


FuncExtractResult FuncExtractor::Extract(const std::set<std::string>& functionInitials)
{
	ULONG cInst;
	IINST *rgInst = NULL;
	if (!m_pbsc->getOverloadArray("*", m_mbf, &rgInst, &cInst) || cInst == 0) {
		printf("no symbols matching '%s'\n", "*");
	}
	
	FuncExtractResult result;
	sregex funcRegex = sregex::compile("(?P<func>.*)\\((?P<params>.*)\\)");
	//select all of the function definitions into result.m_funcDefMap.
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
		//only care about functions and member functions
		if (type == "function" || type == "mem_func")
		{
			FuncDefinition funcDef;

			
			smatch what;
			if (regex_search(fullFuncWithParams, what, funcRegex))
			{
				funcDef.m_fullQualifiedName = what["func"];
				funcDef.m_parameters = what["params"];
				funcDef.m_id = rgInst[i];

				IDEF *rgidef;
				ULONG cidef;
				if (m_pbsc->getDefArray(rgInst[i], &rgidef, &cidef) && cidef != 0) 
				{
					for (auto j = 0; j < cidef; j++) {
						SZ sz; LINE line;
						m_pbsc->idefInfo(rgidef[j], &sz, &line);
						funcDef.m_sourceFilePath = sz;
						funcDef.m_startLine = line;
					}
					m_pbsc->disposeArray(rgidef);
				}

				result.m_funcDefMap[funcDef.m_id] = funcDef;
				
			}

		}
	}
	
	//select the related funcCalls into result.m_funcCallQueue
	std::map<int, FuncDefinition> extraFuncDefMap;
	for (auto itr = result.m_funcDefMap.begin(); itr != result.m_funcDefMap.end(); itr++)
	{
		SZ sz; TYP typ; ATR atr;  LINE line;
		IDEF *rgidef;
		ULONG cidef;
		std::string type;
		if (m_pbsc->getRefArray(itr->first, &rgidef, &cidef) && cidef != 0)
		{
			for (auto j = 0; j < cidef; j++) {
				
				IINST callingContextId = m_pbsc->iinstContextIref(rgidef[j]);
				if (callingContextId != iinstNil)
				{
					m_pbsc->iinstInfo(callingContextId, &sz, &typ, &atr);
					type = m_pbsc->szFrTyp(typ);
					//only care about source context is a function or member function
					if (type != "function" && type != "mem_func")
						continue;
					//if source context is not in the map (external call case), just create it.
					if (result.m_funcDefMap.find(callingContextId) == result.m_funcDefMap.end()
						&& extraFuncDefMap.find(callingContextId) == extraFuncDefMap.end())
					{
						FuncDefinition funcDef;
						IDEF *rgidef_;
						ULONG cidef_;
						funcDef.m_id = callingContextId;
						string fullFuncWithParams = m_pbsc->formatDname(sz);
						smatch what;
						if (regex_search(fullFuncWithParams, what, funcRegex))
						{
							funcDef.m_fullQualifiedName = what["func"];
							funcDef.m_parameters = what["params"];
						}
						if (m_pbsc->getDefArray(callingContextId, &rgidef_, &cidef_) && cidef_ != 0)
						{
							for (auto j = 0; j < cidef_; j++) {
								SZ sz_; LINE line_;
								m_pbsc->idefInfo(rgidef_[j], &sz_, &line_);
								funcDef.m_sourceFilePath = sz_;
								funcDef.m_startLine = line_;
							}
							m_pbsc->disposeArray(rgidef_);
						}
						extraFuncDefMap[callingContextId] = funcDef;
					}
				}
				else
					continue;
				FuncCall funcCall;
				funcCall.m_sourceId = callingContextId;
				funcCall.m_targetId = itr->first;
				m_pbsc->irefInfo(rgidef[j], &sz, &line);
				funcCall.m_sourceFilePath = sz;
				funcCall.m_startLine = line;
				result.m_funcCallVec.push_back(funcCall);
			}
			m_pbsc->disposeArray(rgidef);

		}
	}

	//merge extraFuncDefMap to result.m_funcDefMap
	result.m_funcDefMap.insert(extraFuncDefMap.begin(), extraFuncDefMap.end());
	extraFuncDefMap.clear();

	return result;
}