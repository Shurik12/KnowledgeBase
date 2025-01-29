cmake_minimum_required(VERSION 3.20)

project(knowledgebase VERSION 0.1 DESCRIPTION "My API")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++17")

find_package(clickhouse-cpp REQUIRED)
find_package(double-conversion REQUIRED)
find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(CURL REQUIRED)
find_package(lz4 REQUIRED)
find_package(libpqxx REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(spdlog REQUIRED)

set(src_headers)
set(src_sources)

file(GLOB src_headers 
        "src/YandexMusic/*.h" 
        "src/Common/*.h"
        "src/Databases/*.h")
file(GLOB src_sources 
        "src/YandexMusic/*.cpp" 
        "src/Common/*.cpp" 
        "src/Databases/*.cpp")

set(test_headers)
set(test_sources)

file(GLOB test_headers 
        "src/YandexMusic/*.h" 
        "src/Common/*.h"
        "src/Databases/*.h"
        "test/*.h")
file(GLOB test_sources 
        "src/YandexMusic/*.cpp" 
        "src/Common/*.cpp" 
        "src/Databases/*.cpp"
        "test/*.cpp")

add_executable(knowledgebase main.cpp)
target_sources(knowledgebase PRIVATE ${src_headers} ${src_sources})
target_link_libraries(
    knowledgebase 
    clickhouse-cpp-lib::clickhouse-cpp-lib
    double-conversion::double-conversion
    fmt::fmt
    CURL::libcurl
    LZ4::lz4_static
    libpqxx::pqxx
    rapidjson
    spdlog::spdlog)
target_include_directories(knowledgebase PUBLIC src)


add_executable(server server.cpp)
target_sources(server PRIVATE ${src_headers} ${src_sources})
target_link_libraries(
    server 
    clickhouse-cpp-lib::clickhouse-cpp-lib
    double-conversion::double-conversion
    fmt::fmt
    CURL::libcurl
    LZ4::lz4_static
    libpqxx::pqxx
    rapidjson
    spdlog::spdlog)
target_include_directories(server PUBLIC src)

add_executable(client client.cpp)
target_link_libraries(
    client 
    fmt::fmt)
target_include_directories(client PUBLIC src)

add_executable(tests tests.cpp)
target_sources(tests PRIVATE ${test_headers} ${test_sources})
target_link_libraries(
    tests
    gtest::gtest
    clickhouse-cpp-lib::clickhouse-cpp-lib
    CURL::libcurl
    libpqxx::pqxx
    rapidjson
    spdlog::spdlog)
target_include_directories(tests PUBLIC src test)