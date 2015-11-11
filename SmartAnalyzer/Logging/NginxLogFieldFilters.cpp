#include "NginxLogFieldFilters.h"

using namespace SmartAnalyzer::Logging;
using namespace SmartAnalyzer::Logging::Nginx;

bool RequestLogFieldFilter::Filter(const string& strField) const
{
	//TODO: split the strField into request_method, request_message
	return true;
}

bool StatusLogFieldFilter::Filter(const string& strField) const
{	
	int statusCode = stoi(strField);
	return (m_statusCodeSet.find(statusCode) != m_statusCodeSet.end());
}

bool RemoteAddrLogFieldFilter::Filter(const string& strField) const
{
	//TODO: understand m_addressRange which is used for filtering
	return true;
}