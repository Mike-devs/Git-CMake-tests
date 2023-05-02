#include "BackendHandlerFactory.h"

#include "Poco/Net/HTTPServerRequest.h"

#include "TimeRequestHandler.h"
#include "RegisterRequestHandler.h"
#include "LoginRequestHandler.h"

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;

BackendHandlerFactory::BackendHandlerFactory(const std::string& format) :
	_format(format)
{
}

HTTPRequestHandler* BackendHandlerFactory::createRequestHandler(const HTTPServerRequest& request)
{
	/*Application& app = Application::instance();

	Poco::Thread::TID threadId = Poco::Thread::currentTid();
	app.logger().information("Poco thread id: " + std::string(threadId));

	for (NameValueCollection::ConstIterator&& iter = request.begin(); iter != request.end(); iter++)
	{
		app.logger().information(iter->first + " / " + iter->second);
	}

	if (request.hasCredentials())
	{
		std::string scheme, authInfo;
		request.getCredentials(scheme, authInfo);
		app.logger().information("Credentials: " + scheme + " / " + authInfo);
	}*/

	if (request.getURI() == "/")
		return new TimeRequestHandler(_format);
	else if (request.getURI().starts_with("/register"))
		return new RegisterRequestHandler();
	else if (request.getURI().starts_with("/login"))
		return new LoginRequestHandler();
	else
		return 0;
}