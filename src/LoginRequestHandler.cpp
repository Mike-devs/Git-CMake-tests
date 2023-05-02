#include "LoginRequestHandler.h"

#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/QueryRequest.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

#include "BackendServer.h"

using Poco::Util::Application;
using Poco::Timestamp;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NetException;
using Poco::MongoDB::Connection;
using Poco::MongoDB::QueryRequest;
using Poco::MongoDB::ResponseMessage;
using Poco::MongoDB::Document;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::Dynamic::Var;

LoginRequestHandler::LoginRequestHandler(void)
{
}

void LoginRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	//app.logger().information("Request from " + request.clientAddress().toString());
	BackendServer& backend = dynamic_cast<BackendServer&>(Application::instance());
	Timestamp now;

	if (request.getMethod() != HTTPRequest::HTTP_POST)
	{
		response.setStatusAndReason(HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
		response.send();
	}
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

		QueryRequest dbRequest("altwy.users");
		dbRequest.selector().add("user_id", user_id);
		dbRequest.setNumberToReturn(1);

		ResponseMessage dbResponse;
		backend.fConnection->sendRequest(dbRequest, dbResponse);
		if (dbResponse.documents().size() > 0)
		{
			Object object;

			Document::Ptr doc = dbResponse.documents()[0];
			object.set("lastname", doc->get<std::string>("lastname"));
			object.set("firstname", doc->get<std::string>("firstname"));

			response.setChunkedTransferEncoding(true);
			response.setContentType("application/json");
			object.stringify(response.send());
		}
		else
		{
			response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
			response.send();
		}
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
