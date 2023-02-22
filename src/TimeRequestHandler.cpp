#include "TimeRequestHandler.h"

#include <string>

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"

using Poco::Util::Application;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Timestamp;
using Poco::DateTimeFormatter;

TimeRequestHandler::TimeRequestHandler(const std::string& format) :
_format(format)
{
}

void TimeRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Application& app = Application::instance();
	app.logger().information("Request from " + request.clientAddress().toString());

	Timestamp now;
	std::string dt(DateTimeFormatter::format(now, _format));

	response.setChunkedTransferEncoding(true);
	response.setContentType("text/html");

	std::ostream& ostr = response.send();
	ostr << "<html><head><title>HTTPTimeServer powered by POCO C++ Libraries</title>";
	ostr << "<meta http-equiv=\"refresh\" content=\"1\"></head>";
	ostr << "<body><p style=\"text-align: center; font-size: 48px;\">";
	ostr << dt;
	ostr << "</p></body></html>";
}
