file(GLOB SRC "*.cpp" "*.hpp")

add_library(GraphicalInterface ${SRC})

target_link_libraries(GraphicalInterface PUBLIC imgui PRIVATE physics graphics utils)
target_include_directories(GraphicalInterface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
