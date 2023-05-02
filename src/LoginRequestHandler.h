#pragma once

#include "Poco/Net/HTTPRequestHandler.h"

namespace Poco
{
	namespace Net {
		class HTTPServerRequest;
		class HTTPServerResponse;
	}
}

/// Gets a user id as input and returns first name and last name.
class LoginRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	LoginRequestHandler(void);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
