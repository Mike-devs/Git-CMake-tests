#include "RegisterRequestHandler.h"

#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
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

#include "BackendServer.h"

using Poco::Util::Application;
using Poco::Timestamp;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NetException;
using Poco::MongoDB::Connection;
using Poco::MongoDB::Document;
using Poco::MongoDB::InsertRequest;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::Dynamic::Var;
using Poco::UUIDGenerator;
using Poco::UUID;

RegisterRequestHandler::RegisterRequestHandler(void)
{
}

void RegisterRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
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
		std::string user_id = UUIDGenerator::defaultGenerator().create().toString();

		Document::Ptr user = new Document();
		user->add("lastname", pObject->getValue<std::string>("lastname"));
		user->add("firstname", pObject->getValue<std::string>("firstname"));
		user->add("user_id", user_id);

		InsertRequest dbRequest("altwy.users");
		dbRequest.documents().push_back(user);
		backend.fConnection->sendRequest(dbRequest);

		Object object;
		object.set("user_id", user_id);
	
		response.setChunkedTransferEncoding(true);
		response.setContentType("application/json");
		object.stringify(response.send());
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
