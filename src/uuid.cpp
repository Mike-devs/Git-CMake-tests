#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Exception.h"
#include <iostream>

using Poco::UUID;
using Poco::UUIDGenerator;
using Poco::Exception;

void GeneratePocoUuid(int argc, char** argv)
{
    UUID uuid;

	std::string arg;
	if (argc > 1)
		arg = argv[1];

	try
	{
		if (arg == "-random")
			uuid = UUIDGenerator::defaultGenerator().createRandom();
		else if (arg.empty())
			uuid = UUIDGenerator::defaultGenerator().create();
		else
			uuid = UUIDGenerator::defaultGenerator().createFromName(UUID::uri(), arg);

		UUID::Version vers = uuid.version();
		std::cout << uuid.toString() << std::endl;
	}
	catch (Exception& exc)
	{
		std::cerr << exc.displayText() << std::endl;
	}
}