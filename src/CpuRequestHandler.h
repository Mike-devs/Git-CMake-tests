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
class CpuRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	CpuRequestHandler(void);
	~CpuRequestHandler(void);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
