#option(ENABLE_RAPIDJSON "Use rapidjson" ${ENABLE_LIBRARIES})
#
#if(NOT ENABLE_RAPIDJSON)
#    message(STATUS "Not using rapidjson")
#    return()
#endif()

set(RAPIDJSON_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/contrib/rapidjson/include")
add_library(_rapidjson INTERFACE)
target_include_directories(_rapidjson SYSTEM BEFORE INTERFACE ${RAPIDJSON_INCLUDE_DIR})
add_library(kb_contrib::rapidjson ALIAS _rapidjson)
