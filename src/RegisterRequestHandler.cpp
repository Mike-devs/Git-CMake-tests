#include "RegisterRequestHandler.h"

#include "Poco/Util/Application.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/NetException.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/InsertRequest.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/UUID.h"

using Poco::Util::Application;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NetException;
using Poco::MongoDB::Connection;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::Dynamic::Var;
using Poco::UUIDGenerator;
using Poco::UUID;

RegisterRequestHandler::RegisterRequestHandler(Poco::SharedPtr<Poco::MongoDB::Connection> connection)
{
	fConnection = connection;
}

void RegisterRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	Application& app = Application::instance();
	app.logger().information("Request from " + request.clientAddress().toString());

	response.setChunkedTransferEncoding(true);
	response.setContentType("application/json");

	if (request.getMethod() != HTTPRequest::HTTP_POST)
	{
		response.setStatusAndReason(Poco::Net::HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
		std::ostream& ostr = response.send();
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
		std::string user_id = UUIDGenerator::defaultGenerator().create().toString();

		Poco::MongoDB::Document::Ptr user = new Poco::MongoDB::Document();
		user->add("lastname", pObject->getValue<std::string>("lastname"));
		user->add("firstname", pObject->getValue<std::string>("firstname"));
		user->add("user_id", user_id);

		Poco::MongoDB::InsertRequest dbRequest("altwy.users");
		dbRequest.documents().push_back(user);
		fConnection->sendRequest(dbRequest);

		std::ostream& ostr = response.send();
		ostr << "{\"user_id\":\"" << user_id << "\"}";
	}
}
