cmake_minimum_required(VERSION 3.10)
project(Tests VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(ThirdParties/poco)

add_executable(tests src/tests.cpp)
target_link_libraries(tests PUBLIC Poco::Foundation Poco::MongoDB)
