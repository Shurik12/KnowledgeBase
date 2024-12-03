cmake_minimum_required(VERSION 3.20)

project(knowledgebase VERSION 0.1 DESCRIPTION "My API")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++17")

find_package(clickhouse-cpp REQUIRED)
find_package(c-ares REQUIRED)
find_package(double-conversion REQUIRED)
find_package(fmt REQUIRED)
find_package(CURL REQUIRED)
find_package(lz4 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Poco REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(zlib-ng REQUIRED)

set(src_headers)
set(src_sources)

file(GLOB src_headers "src/YandexMusic/*.h" "src/Common/*.h")
file(GLOB src_sources "src/YandexMusic/*.cpp" "src/Common/*.cpp")

# add_executable(knowledgebase main.cpp)
# target_link_libraries(knowledgebase yandex_music)
# target_include_directories(knowledgebase PUBLIC src base contrib)

add_executable(server server.cpp)
target_sources(server PRIVATE ${src_headers} ${src_sources})
target_link_libraries(
    server 
    clickhouse-cpp-lib::clickhouse-cpp-lib
    c-ares::cares
    double-conversion::double-conversion
    fmt::fmt
    CURL::libcurl
    LZ4::lz4_static
    openssl::openssl
    Poco::XML
    Poco::Foundation
    Poco::Net
    rapidjson
    zlib-ng::zlib-ng)
target_include_directories(server PUBLIC src)

add_executable(client client.cpp)
target_sources(server PRIVATE ${src_headers} ${src_sources})
target_link_libraries(
    client 
    fmt::fmt)
target_include_directories(client PUBLIC src)

