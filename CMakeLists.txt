cmake_minimum_required(VERSION 3.20)

project(knowledgebase VERSION 0.1 LANGUAGES CXX DESCRIPTION "My API")

# Set C++ standard and properties
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif(MSVC)
    add_compile_options(/W4 /WX)
endif()

# Project options
option(BUILD_TESTS "Build tests" ON)
option(BUILD_CLIENT "Build client" ON)

# Find dependencies
find_package(clickhouse-cpp REQUIRED)
find_package(double-conversion REQUIRED)
find_package(fmt REQUIRED)
find_package(CURL REQUIRED)
find_package(lz4 REQUIRED)
find_package(libpqxx REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(spdlog REQUIRED)

if(BUILD_TESTS)
    find_package(GTest REQUIRED)
endif()

# Source file organization
set(SRC_DIRS 
    src/YandexMusic
    src/Common
    src/Databases
    src/Server
)

# Collect source files properly
set(src_headers)
set(src_sources)

foreach(DIR ${SRC_DIRS})
    file(GLOB HEADERS "${DIR}/*.h" "${DIR}/*.hpp")
    file(GLOB SOURCES "${DIR}/*.cpp")
    list(APPEND src_headers ${HEADERS})
    list(APPEND src_sources ${SOURCES})
endforeach()

# Server executable - using target_sources() for better modularity
add_executable(server server.cpp)
target_sources(server PRIVATE ${src_sources})
target_include_directories(server PUBLIC src)
target_link_libraries(server PRIVATE
    clickhouse-cpp-lib::clickhouse-cpp-lib
    double-conversion::double-conversion
    fmt::fmt
    CURL::libcurl
    LZ4::lz4_static
    libpqxx::pqxx
    rapidjson
    spdlog::spdlog
)

# Header files are included as part of the target for IDE support
# but they don't need to be compiled separately
set_target_properties(server PROPERTIES
    LINKER_LANGUAGE CXX
)

# Client executable (conditional)
if(BUILD_CLIENT)
    add_executable(client client.cpp)
    target_include_directories(client PUBLIC src)
    target_link_libraries(client PRIVATE 
        fmt::fmt
        rapidjson
    )
endif()

# Tests executable (conditional)
if(BUILD_TESTS)
    set(TEST_DIRS ${SRC_DIRS})
    set(TEST_ADDITIONAL_DIRS test)
    
    set(test_sources)
    
    # Add main source files
    foreach(DIR ${TEST_DIRS})
        file(GLOB SOURCES "${DIR}/*.cpp")
        list(APPEND test_sources ${SOURCES})
    endforeach()
    
    # Add test-specific source files
    foreach(DIR ${TEST_ADDITIONAL_DIRS})
        file(GLOB HEADERS "${DIR}/*.h" "${DIR}/*.hpp")
        file(GLOB SOURCES "${DIR}/*.cpp")
        list(APPEND test_headers ${HEADERS})
        list(APPEND test_sources ${SOURCES})
    endforeach()
    
    add_executable(tests tests.cpp)
    target_sources(tests PRIVATE ${test_sources})
    target_include_directories(tests PUBLIC src test)
    target_link_libraries(tests PRIVATE
        gtest::gtest
        clickhouse-cpp-lib::clickhouse-cpp-lib
        CURL::libcurl
        libpqxx::pqxx
        rapidjson
        spdlog::spdlog
    )
endif()

# Installation targets
install(TARGETS server
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if(BUILD_CLIENT)
    install(TARGETS client
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()