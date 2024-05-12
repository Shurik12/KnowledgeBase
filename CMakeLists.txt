cmake_minimum_required(VERSION 3.20)

project(knowledgebase VERSION 0.1 DESCRIPTION "My API")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -std=c++17")

add_subdirectory(src)
add_subdirectory(base)
add_subdirectory(contrib EXCLUDE_FROM_ALL)

add_executable(knowledgebase main.cpp)
target_link_libraries(knowledgebase yandex_music)
target_include_directories(knowledgebase PUBLIC src base contrib)
