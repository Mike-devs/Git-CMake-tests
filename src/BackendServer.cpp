#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/NetException.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/MongoDB/Connection.h"

#include <iostream>

#include "TimeRequestHandler.h"
#include "RegisterRequestHandler.h"
#include "LoginRequestHandler.h"

using Poco::Net::ServerSocket;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Net::NameValueCollection;
using Poco::DateTimeFormat;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::MongoDB::Connection;

class BackendHandlerFactory: public HTTPRequestHandlerFactory
{
public:
	BackendHandlerFactory(const std::string& format):
	_format(format)
	{
		try
		{
			fConnection = new Connection("127.0.0.1", 27017);
			std::cout << "Connected to MongoDB 127.0.0.1:27017" << std::endl;
		}
		catch (Poco::Net::ConnectionRefusedException& e)
		{
			std::cout << "Couldn't connect to MongoDB 127.0.0.1:27017: " << e.message() << std::endl;
		}
	}

	HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request)
	{
		Application& app = Application::instance();

		Poco::Thread::TID threadId = Poco::Thread::currentTid();
		//app.logger().information("Poco thread id: " + std::string(threadId));

		for (NameValueCollection::ConstIterator&& iter = request.begin(); iter != request.end(); iter++)
		{
			app.logger().information(iter->first + " / " + iter->second);
		}

		if (request.hasCredentials())
		{
			std::string scheme, authInfo;
			request.getCredentials(scheme, authInfo);
			app.logger().information("Credentials: " + scheme + " / " + authInfo);
		}

		if (request.getURI() == "/")
			return new TimeRequestHandler(_format);
		else if (request.getURI().starts_with("/register"))
			return new RegisterRequestHandler(fConnection);
		else if (request.getURI().starts_with("/login"))
			return new LoginRequestHandler(fConnection);
		else
			return 0;
	}

private:
	std::string _format;
	Poco::SharedPtr<Poco::MongoDB::Connection> fConnection;
};


class BackendServer: public Poco::Util::ServerApplication
	/// The main application class.
	///
	/// This class handles command-line arguments and
	/// configuration files.
	/// Start the HTTPTimeServer executable with the help
	/// option (/help on Windows, --help on Unix) for
	/// the available command line options.
	///
	/// To use the sample configuration file (HTTPTimeServer.properties),
	/// copy the file to the directory where the HTTPTimeServer executable
	/// resides. If you start the debug version of the HTTPTimeServer
	/// (HTTPTimeServerd[.exe]), you must also create a copy of the configuration
	/// file named HTTPTimeServerd.properties. In the configuration file, you
	/// can specify the port on which the server is listening (default
	/// 9980) and the format of the date/time string sent back to the client.
	///
	/// To test the TimeServer you can use any web browser (http://localhost:9980/).
{
public:
	BackendServer()
	{
	}

	~BackendServer()
	{
	}

protected:
	void initialize(Application& self)
	{
		loadConfiguration(); // load default configuration files, if present
		ServerApplication::initialize(self);
	}

	void uninitialize()
	{
		ServerApplication::uninitialize();
	}

	void defineOptions(OptionSet& options)
	{
		ServerApplication::defineOptions(options);
	}

	void handleOption(const std::string& name, const std::string& value)
	{
		ServerApplication::handleOption(name, value);
	}

	int main(const std::vector<std::string>& args)
	{
		// get parameters from configuration file
		unsigned short port = (unsigned short) config().getInt("HTTPTimeServer.port", 9980);
		std::string format(config().getString("HTTPTimeServer.format", DateTimeFormat::SORTABLE_FORMAT));
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
};


void LaunchTimeServer(int argc, char** argv)
{
	BackendServer app;
	app.run(argc, argv);
}
