#include "LogRequestHandler.h"

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

LogRequestHandler::LogRequestHandler(void)
{
}

LogRequestHandler::~LogRequestHandler()
{
}

void LogRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	BackendServer& backend = dynamic_cast<BackendServer&>(Application::instance());

	if (request.getMethod() == HTTPRequest::HTTP_DELETE)
	{
		backend.fStats.Reset();

		response.setChunkedTransferEncoding(true);
		response.setContentType("application/json");

		Object::Ptr object = new Object();
		object->set("done", true);
		object->stringify(response.send());
	}
	else if (request.getMethod() == HTTPRequest::HTTP_GET)
	{
		std::cout << "##### Stats #####" << std::endl;
		std::cout << "> Processed Requests : " << backend.fStats.processedRequestsCount << std::endl;
		std::cout << "> Average Process Time : " << backend.fStats.averageRequestProcessTimeMs << "ms" << std::endl;
		std::cout << "#################" << std::endl;

		response.setChunkedTransferEncoding(true);
		response.setContentType("application/json");

		Object::Ptr object = new Object();
		Object::Ptr stats = new Object();
		stats->set("processedRequestsCount", backend.fStats.processedRequestsCount);
		stats->set("averageRequestProcessTimeMs", backend.fStats.averageRequestProcessTimeMs);
		object->set("stats", stats);
		object->stringify(response.send());
	}
	else
	{
		response.setStatusAndReason(HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
		response.send();
	}
}
