cmake_minimum_required(VERSION 3.10)
project(Tests VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Poco REQUIRED Foundation Net Util)
find_package (mongoc-1.0 REQUIRED)

add_executable(tests src/tests.cpp src/uuid.cpp src/mongo_hello.cpp src/HTTPTimeServer.cpp)
target_link_libraries(tests PUBLIC Poco::Foundation Poco::Net Poco::Util mongo::mongoc_shared)
