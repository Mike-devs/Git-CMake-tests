#pragma once

#include "Poco/Net/HTTPRequestHandler.h"

namespace Poco
{
	namespace Net {
		class HTTPServerRequest;
		class HTTPServerResponse;
	}
}

/// Gets a first name and last name as input and returns an user id.
class RegisterRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	RegisterRequestHandler(void);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};
