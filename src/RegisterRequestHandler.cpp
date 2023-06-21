#include "RegisterRequestHandler.h"

#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/UUID.h"

#include "bsoncxx/builder/basic/document.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/exception/bulk_write_exception.hpp"

#include "BackendServer.h"

using Poco::Util::Application;
using Poco::Timestamp;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::Dynamic::Var;
using Poco::UUIDGenerator;
using Poco::UUID;

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

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

		bool error = false;
		try
		{
			auto db = backend.fConnection["altwy"];
			auto insertResult = db["users"].insert_one(make_document(
				kvp("firstname", pObject->getValue<std::string>("firstname")),
				kvp("lastname", pObject->getValue<std::string>("lastname")),
				kvp("user_id", user_id)));
			//bsoncxx::types::b_oid oid = insertResult.value().inserted_id().get_oid();
			//std::cout << "inserted id: " << oid.value.to_string();
		}
		catch(mongocxx::bulk_write_exception& e)
		{
			error = true;
			std::cout << e.what() << std::endl;
			response.send() << "mongocxx::bulk_write_exception: " << e.what();
		}
		catch (std::exception& e)
		{
			error = true;
			std::cout << e.what() << std::endl;
			response.send() << "std::exception: " << e.what();
		}
		catch (...)
		{
			error = true;
			std::cout << "Unknown exception" << std::endl;
			response.send() << "Unknown exception";
		}

		if(!error)
		{
			Object object;
			object.set("user_id", user_id);

			response.setChunkedTransferEncoding(true);
			response.setContentType("application/json");
			object.stringify(response.send());
		}
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
