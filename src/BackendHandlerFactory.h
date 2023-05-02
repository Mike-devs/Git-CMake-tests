#pragma once

#include "Poco/Net/HTTPRequestHandlerFactory.h"

namespace Poco {
	namespace Net {
		class HTTPRequestHandler;
		class HTTPServerRequest;
	}
}
class BackendHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	BackendHandlerFactory(const std::string& format);

	Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest& request);


private:
	std::string _format;
};