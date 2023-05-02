#include "TimeRequestHandler.h"

#include <string>

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/JSON/Object.h"

#include "BackendServer.h"

using Poco::Util::Application;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Timestamp;
using Poco::DateTimeFormatter;
using Poco::JSON::Object;

TimeRequestHandler::TimeRequestHandler(const std::string& format) :
	_format(format)
{
}

TimeRequestHandler::~TimeRequestHandler()
{
}

void TimeRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	//app.logger().information("Request from " + request.clientAddress().toString());
	BackendServer& backend = dynamic_cast<BackendServer&>(Application::instance());
	Timestamp now;

	if (request.getMethod() != HTTPRequest::HTTP_GET)
	{
		response.setStatusAndReason(HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
		response.send();
	}
	else
	{
		Object::Ptr object = new Object();

		response.setChunkedTransferEncoding(true);
		response.setContentType("application/json");

		object->set("time", DateTimeFormatter::format(now, _format));
		object->stringify(response.send());
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
