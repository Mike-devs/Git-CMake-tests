cmake_minimum_required(VERSION 3.10)
project(Tests VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Poco REQUIRED Foundation Net Util MongoDB)
find_package (mongoc-1.0 REQUIRED)
find_package (mongocxx REQUIRED)
find_package (glaze REQUIRED)

add_executable(tests	src/tests.cpp
						src/BackendServer.cpp src/BackendHandlerFactory.cpp
						src/RegisterRequestHandler.cpp src/LoginRequestHandler.cpp src/TimeRequestHandler.cpp
						src/LogRequestHandler.cpp src/CpuRequestHandler.cpp)
target_link_libraries(tests PUBLIC	Poco::Foundation Poco::Net Poco::Util Poco::MongoDB
									mongo::mongoc_shared mongo::mongocxx_shared
									glaze::glaze)
