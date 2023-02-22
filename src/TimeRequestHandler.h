#pragma once

#include "Poco/Net/HTTPRequestHandler.h"

class Poco::Net::HTTPServerRequest;
class Poco::Net::HTTPServerResponse;

class TimeRequestHandler : public Poco::Net::HTTPRequestHandler
	/// Return a HTML document with the current date and time.
{
public:
	TimeRequestHandler(const std::string& format);

	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);

private:
	std::string _format;
};
