#include "LoginRequestHandler.h"

#include "Poco/Util/Application.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

using Poco::Util::Application;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NetException;
using Poco::MongoDB::Connection;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::Dynamic::Var;

LoginRequestHandler::LoginRequestHandler(Poco::SharedPtr<Poco::MongoDB::Connection> connection)
{
	fConnection = connection;
}

void LoginRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Application& app = Application::instance();
	app.logger().information("Request from " + request.clientAddress().toString());

	response.setChunkedTransferEncoding(true);
	response.setContentType("application/json");

	if (request.getMethod() != HTTPRequest::HTTP_POST)
		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
	else
	{
		std::streamsize length = request.getContentLength();
		std::istream& stream = request.stream();
		std::string body;
		body.resize(length);
		stream.read(&body[0], length);

		Parser parser;
		Var result = parser.parse(body);
		Object::Ptr pObject = result.extract<Object::Ptr>();
		std::string user_id = pObject->getValue<std::string>("user_id");

		Poco::MongoDB::QueryRequest dbRequest("altwy.users");
		dbRequest.selector().add("user_id", user_id);
		dbRequest.setNumberToReturn(1);

		std::ostream& ostr = response.send();
		Poco::MongoDB::ResponseMessage dbResponse;
		fConnection->sendRequest(dbRequest, dbResponse);
		if (dbResponse.documents().size() > 0)
		{
			Poco::MongoDB::Document::Ptr doc = dbResponse.documents()[0];
			ostr << "{\"lastname\":" << doc->get<std::string>("lastname") << "," << "\"firstname\":" << doc->get<std::string>("firstname") << "}";
		}
		else
			response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
	}
}
