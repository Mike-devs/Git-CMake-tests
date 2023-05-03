#include "BackendServer.h"

#include "Poco/DateTimeFormat.h"
#include "Poco/ThreadPool.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/Net/NetException.h"

#include "BackendHandlerFactory.h"

using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Net::ServerSocket;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::ConnectionRefusedException;
using Poco::Util::Application;
using Poco::Util::ServerApplication;
using Poco::Util::OptionSet;
using Poco::MongoDB::Connection;

using ConnectionPtr = Poco::SharedPtr<Poco::MongoDB::Connection>;

BackendServer::BackendServer()
{
	try
	{
		fConnection = new Connection("127.0.0.1", 27018);
		std::cout << "Connected to MongoDB 127.0.0.1:27018" << std::endl;
	}
	catch (ConnectionRefusedException& e)
	{
		std::cout << "Couldn't connect to MongoDB 127.0.0.1:27018: " << e.message() << std::endl;
	}
}

BackendServer::~BackendServer()
{
}

void BackendServer::initialize(Application& self)
{
	loadConfiguration(); // load default configuration files, if present
	ServerApplication::initialize(self);
}

void BackendServer::uninitialize()
{
	ServerApplication::uninitialize();
}

void BackendServer::defineOptions(OptionSet& options)
{
	ServerApplication::defineOptions(options);
}

void BackendServer::handleOption(const std::string& name, const std::string& value)
{
	ServerApplication::handleOption(name, value);
}

int BackendServer::main(const std::vector<std::string>& args)
{
	// get parameters from configuration file
	unsigned short port = (unsigned short) config().getInt("HTTPTimeServer.port", 9980);
	std::string format(config().getString("HTTPTimeServer.format", DateTimeFormat::HTTP_FORMAT));
	int maxQueued  = config().getInt("HTTPTimeServer.maxQueued", 100);
	int maxThreads = config().getInt("HTTPTimeServer.maxThreads", 16);
	ThreadPool::defaultPool().addCapacity(maxThreads);

	HTTPServerParams* pParams = new HTTPServerParams;
	pParams->setMaxQueued(maxQueued);
	pParams->setMaxThreads(maxThreads);
	Poco::Timespan timeout(5, 0);
	pParams->setTimeout(timeout);

	// set-up a server socket
	ServerSocket svs(port);
	// set-up a HTTPServer instance
	HTTPServer srv(new BackendHandlerFactory(format), svs, pParams);
	// start the HTTPServer
	srv.start();
	// wait for CTRL-C or kill
	waitForTerminationRequest();
	// Stop the HTTPServer
	srv.stop();

	return Application::EXIT_OK;
}


void LaunchTimeServer(int argc, char** argv)
{
	BackendServer app;
	app.run(argc, argv);
}
