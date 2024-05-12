set(src_headers)
set(src_sources)
set(LIBS kb_contrib::curl kb_contrib::rapidjson Poco::XML Poco::Net Poco::Foundation kb_contrib::fmt)

file(GLOB src_headers "YandexMusic/*.h" "Common/*.h")
file(GLOB src_sources "YandexMusic/*.cpp" "Common/*.cpp")

add_library (yandex_music STATIC ${src_headers} ${src_sources})
target_link_libraries(yandex_music ${LIBS})
target_include_directories(yandex_music PUBLIC ${CMAKE_SOURCE_DIR}/src)
