# main app launcher cmake file
add_executable(mainSimulator fluidSimulator.cpp fluidSimulator.h)
target_link_libraries(mainSimulator PUBLIC src)

if(APPLE)
    find_library(OpenGL_Framework OpenGL)
    target_link_libraries(mainSimulator PRIVATE ${OpenGL_Framework})
elseif()
    find_package(OpenGL REQUIRED CONFIG)
    target_link_libraries(mainSimulator PRIVATE OpenGL)
endif()

if(WIN32)
    set_target_properties(mainSimulator PROPERTIES LINK_FLAGS "/ignore:4099")
endif()