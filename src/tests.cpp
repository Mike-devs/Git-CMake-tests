//
// uuidgen.cpp
//
// This sample demonstrates the UUIDGenerator and UUID classes.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/UUID.h"
#include "Poco/UUIDGenerator.h"
#include "Poco/Exception.h"
#include <iostream>

#include "Poco/MongoDB/MongoDB.h"
#include "Poco/MongoDB/Connection.h"
#include "Poco/MongoDB/Cursor.h"

using Poco::UUID;
using Poco::UUIDGenerator;
using Poco::Exception;


int main(int argc, char** argv)
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
		return 1;
	}

	Poco::MongoDB::Connection connection("localhost", 27017);
	Poco::MongoDB::Cursor cursor("xtralife", "games");
	// Selecting fields is done by adding them to the returnFieldSelector
	// Use 1 as value of the element.
	//cursor.query().returnFieldSelector().add("appid", 1);
	Poco::MongoDB::ResponseMessage& response = cursor.next(connection);
	for (;;)
	{
		for (Poco::MongoDB::Document::Vector::const_iterator it = response.documents().begin(); it != response.documents().end(); ++it)
		{
			std::cout << (*it)->get<std::string>("appid") << std::endl;
		}

		// When the cursorID is 0, there are no documents left, so break out ...
		if (response.cursorID() == 0)
		{
			break;
		}

		// Get the next bunch of documents
		response = cursor.next(connection);
	}

	return 0;
}
