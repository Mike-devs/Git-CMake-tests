#pragma once

#include "Poco/SharedPtr.h"
#include "Poco/Net/HTTPRequestHandler.h"

class Poco::Net::HTTPServerRequest;
class Poco::Net::HTTPServerResponse;

namespace Poco {
	namespace MongoDB {
		class Connection;
	}
}

class RegisterRequestHandler : public Poco::Net::HTTPRequestHandler
	/// Return a HTML document with the current date and time.
{
public:
	RegisterRequestHandler(Poco::SharedPtr<Poco::MongoDB::Connection> connection);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
	Poco::SharedPtr<Poco::MongoDB::Connection> fConnection;
};
