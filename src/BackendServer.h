#pragma once

#include "Poco/Util/ServerApplication.h"

#include "mongocxx/instance.hpp"
#include "mongocxx/client.hpp"


namespace Poco {
	namespace Util {
		class OptionSet;
	}
	/* Mongo poco
	namespace MongoDB {
		class Connection;
	}
	Mongo poco */
}

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
	BackendServer();
	~BackendServer();

	
	/* Mongo poco
	Poco::SharedPtr<Poco::MongoDB::Connection> fConnection;
	Mongo poco */
	const mongocxx::instance fMongoInstance{};
	mongocxx::client fConnection{mongocxx::uri{}};
	class Stats {
	public:
		Stats(void)
		{
			Reset();
		}
		void Reset(void)
		{
			processedRequestsCount = 0;
			averageRequestProcessTimeMs = 0;
		}
		void Update(Poco::Int64 requestProcessTimeMs)
		{
			processedRequestsCount++;
			averageRequestProcessTimeMs = (((processedRequestsCount - 1) * averageRequestProcessTimeMs) + requestProcessTimeMs) / processedRequestsCount;
		}
		int processedRequestsCount;
		Poco::Int64 averageRequestProcessTimeMs;

	} fStats;

protected:
	void initialize(Poco::Util::Application& self);
	void uninitialize();
	void defineOptions(Poco::Util::OptionSet& options);
	void handleOption(const std::string& name, const std::string& value);
	int main(const std::vector<std::string>& args);
};
