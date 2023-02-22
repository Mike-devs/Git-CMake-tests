cmake_minimum_required(VERSION 3.10)
project(Tests VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Poco REQUIRED Foundation Net Util MongoDB)
find_package (mongoc-1.0 REQUIRED)

add_executable(tests src/tests.cpp src/uuid.cpp src/mongo_hello.cpp src/BackendServer.cpp src/RegisterRequestHandler.cpp src/LoginRequestHandler.cpp src/TimeRequestHandler.cpp)
target_link_libraries(tests PUBLIC Poco::Foundation Poco::Net Poco::Util Poco::MongoDB mongo::mongoc_shared)
