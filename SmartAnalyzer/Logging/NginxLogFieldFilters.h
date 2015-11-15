#include "ILogFieldFilter.h"
#include <boost/xpressive/xpressive.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
using namespace std;
using namespace boost::xpressive;
namespace SmartAnalyzer
{
	namespace Logging
	{
		namespace Nginx
		{


			const string REQUEST_FILTER_NAME = "request";
			const string STATUS_FILTER_NAME = "status";
			const string REMOTE_ADDRESS_FILTER_NAME = "remoteAddr";

			const string ANY_REQUEST_METHOD = "*";
			const string ANY_REQUEST_MESSAGE = "*";


			class RequestLogFieldFilter : public ILogFieldFilter
			{
			public:
				static shared_ptr<RequestLogFieldFilter> MatchCreate(const string& token)
				{
					sregex filterRegex = sregex::compile("request=(?P<method>.+)\\s(?P<message>.+)");
					smatch what;
					shared_ptr<RequestLogFieldFilter> pRequestLogFieldFilter(NULL);
					if (regex_search(token, what, filterRegex))
					{
						pRequestLogFieldFilter.reset(new RequestLogFieldFilter(what["request"], what["message"]));
					}
					return pRequestLogFieldFilter;
				}
				RequestLogFieldFilter(const string& requestMethod, const string& requestMessage)
					: m_requestMethod(requestMethod), m_requestMessage(requestMessage)
				{};
				virtual ~RequestLogFieldFilter() {};
				virtual bool Filter(const string& strField) const;
				virtual string GetName() const
				{
					return REQUEST_FILTER_NAME;
				};
			private:
				string m_requestMethod;
				string m_requestMessage;
			};

			class StatusLogFieldFilter : public ILogFieldFilter
			{
			public:
				static shared_ptr<Nginx::StatusLogFieldFilter> MatchCreate(const string& token)
				{
					sregex filterRegex = sregex::compile("status=\\{(?P<status>(\\d+,)*\\d+)\\}");
					smatch what;
					shared_ptr<StatusLogFieldFilter> pStatusLogFieldFilter(NULL);
					if (regex_search(token, what, filterRegex))
					{
						vector<string> resVec;
						boost::split(resVec, what["status"], boost::is_any_of(","));
						std::set<int> statusCodeSet;
						for (auto resStr : resVec)
							statusCodeSet.insert(stoi(resStr));
						pStatusLogFieldFilter.reset(new StatusLogFieldFilter(statusCodeSet));
					}
					return pStatusLogFieldFilter;
				}
				StatusLogFieldFilter(const std::set<int>& statusCodeSet)
				{
					m_statusCodeSet = statusCodeSet;
				}
				virtual ~StatusLogFieldFilter() {};
				virtual bool Filter(const string& strField) const;
				virtual string GetName() const
				{
					return STATUS_FILTER_NAME;
				};
			private:
				std::set<int> m_statusCodeSet;
			};

			class RemoteAddrLogFieldFilter : public ILogFieldFilter
			{
			public:
				RemoteAddrLogFieldFilter(const string& remoteAddressRange)
				{
					
				}
				virtual ~RemoteAddrLogFieldFilter() {};
				virtual bool Filter(const string& strField) const;
				virtual string GetName() const
				{
					return REMOTE_ADDRESS_FILTER_NAME;
				};

			private:
				string m_remoteAddressRange;
			};
		}
	}
}
