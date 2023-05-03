#pragma once

#include "Poco/Net/HTTPRequestHandler.h"

namespace Poco
{
	namespace Net {
		class HTTPServerRequest;
		class HTTPServerResponse;
	}
}

/// Return a HTML document with the current date and time.
class LogRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	LogRequestHandler(void);
	~LogRequestHandler(void);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
