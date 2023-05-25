#include "CpuRequestHandler.h"

#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Util/Application.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Base64Encoder.h"
#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/JSON/Array.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Random.h"
#include "glaze/glaze.hpp"

#include "BackendServer.h"

using Poco::Util::Application;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Timestamp;
using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::Base64Encoder;
using Poco::DateTimeFormatter;
using Poco::JSON::Parser;
using Poco::JSON::Object;
using Poco::JSON::Array;
using Poco::Dynamic::Var;
using Poco::Random;

CpuRequestHandler::CpuRequestHandler(void)
{
}

CpuRequestHandler::~CpuRequestHandler()
{
}

void CpuRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
	BackendServer& backend = dynamic_cast<BackendServer&>(Application::instance());
	Timestamp now;

	if (request.getMethod() == HTTPRequest::HTTP_POST)
	{
		Timestamp now;

		std::streamsize bodyLength = request.getContentLength();
		std::string bodyString;
		bodyString.resize(bodyLength);
		request.stream().read(&bodyString[0], bodyLength);
		Parser parser;
		Var result = parser.parse(bodyString);
		Object::Ptr bodyJson = result.extract<Object::Ptr>();

		response.setChunkedTransferEncoding(true);
		response.setContentType("application/json");

		std::stringstream encodedStr;
		Base64Encoder encoder(encodedStr);
		encoder << bodyString;
		encoder.close();

		Object::Ptr responseJson = new Object;
		responseJson->set("base64", encodedStr.str());
		responseJson->set("time", DateTimeFormatter::format(now, DateTimeFormat::HTTP_FORMAT));

		std::string buffer{};
		if(bodyJson->has("randnums"))
		{
			Random	rng;
			std::vector<float> randArray;

			int randnums = bodyJson->getValue<int>("randnums");
			if (randnums > 0)
				responseJson->set("randnums", randArray);
			else
				randnums = -randnums;
			randArray.reserve(randnums);

			for (int scan = 0; scan < randnums; scan++)
			{
				randArray.push_back(rng.nextFloat());
				//randArray->add(rng.nextFloat());
			}
			glz::write_json(randArray, buffer);
		}
		response.send() << buffer;
	}
	else
	{
		response.setStatusAndReason(HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
		response.send();
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
