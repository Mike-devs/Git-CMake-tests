#include "LoginRequestHandler.h"

#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/Dynamic/Var.h"

#include "bsoncxx/builder/basic/document.hpp"
#include "mongocxx/client.hpp"
#include "mongocxx/exception/query_exception.hpp"

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

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

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

		bool error = false;
		try
		{
			auto val = backend.fConnection["altwy"]["users"].find_one(make_document(kvp("user_id", user_id)));
			if (val)
			{
				Object object;
				bsoncxx::document::view view = *val;
				object.set("firstname", view["firstname"].get_string().value.data());
				object.set("lastname", view["lastname"].get_string().value.data());

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
		catch(mongocxx::query_exception& e)
		{
			error = true;
			std::cout << e.what() << std::endl;
			response.send() << "mongocxx::query_exception: " << e.what();
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
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
