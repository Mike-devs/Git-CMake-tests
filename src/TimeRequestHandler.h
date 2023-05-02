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
class TimeRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	TimeRequestHandler(const std::string& format);
	~TimeRequestHandler();

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
	std::string _format;
};
