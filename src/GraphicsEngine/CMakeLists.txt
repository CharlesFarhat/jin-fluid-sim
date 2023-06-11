file(GLOB SRC "*.cpp" "*.h")

add_library(graphics ${SRC})

target_link_libraries(graphics PUBLIC glad PRIVATE utils)
target_include_directories(graphics INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
