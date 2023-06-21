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
using Poco::Dynamic::Var;
using Poco::Random;

struct CPUResponseBase
{
	std::string base64, time;
};
struct CPUResponseFull : public CPUResponseBase
{
	std::vector<float> randnums;
};

// Since the response is potentially a big JSON, we use glaze instead of Poco::JSON
// glaze is extremely fast, but rigid since it needs a more or less 1 to 1 struct
// with the JSON
template <>
struct glz::meta<CPUResponseBase> {
	using T = CPUResponseBase;
	static constexpr auto value = object(
		"base64", [](auto&& self) -> auto& { return self.base64; },
		"time", [](auto&& self) -> auto& { return self.time; }
	);
};
template <>
struct glz::meta<CPUResponseFull> {
	static constexpr auto value = object(
		"base64", [](auto&& self) -> auto& { return self.base64; },
		"time", [](auto&& self) -> auto& { return self.time; },
		"randnums", [](auto&& self) -> auto& { return self.randnums; }
	);
};

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

		bool genRandnums = bodyJson->has("randnums");
		bool sendRandnums = genRandnums && (bodyJson->getValue<int>("randnums") > 0);
		CPUResponseBase* cpuResponse = genRandnums ? new CPUResponseFull : new CPUResponseBase;
		cpuResponse->base64 = encodedStr.str();
		cpuResponse->time = DateTimeFormatter::format(now, DateTimeFormat::HTTP_FORMAT);

		if(genRandnums)
		{
			Random	rng;
			int randnums = bodyJson->getValue<int>("randnums");
			CPUResponseFull* cpuResponseFull = static_cast<CPUResponseFull*>(cpuResponse);
			if (randnums <= 0)
				randnums = -randnums;
			cpuResponseFull->randnums.reserve(randnums);

			for (int scan = 0; scan < randnums; scan++)
				cpuResponseFull->randnums.push_back(rng.nextFloat());
		}

		std::string buffer{};
		if (sendRandnums)
			glz::write_json(*static_cast<CPUResponseFull*>(cpuResponse), buffer);
		else
			glz::write_json(*cpuResponse, buffer);
		response.send() << buffer;
	}
	else
	{
		response.setStatusAndReason(HTTPResponse::HTTP_METHOD_NOT_ALLOWED);
		response.send();
	}

	backend.fStats.Update(now.elapsed() / 1000);
}
