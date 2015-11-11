#include "ILogFieldFilter.h"
#include <boost/xpressive/xpressive.hpp>
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
