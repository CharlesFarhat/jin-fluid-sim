# main app launcher cmake file
add_executable(mainSimulator fluidSimulator.cpp fluidSimulator.h)

add_subdirectory(imgui_backend)

target_link_libraries(mainSimulator PRIVATE GraphicalInterface physics graphics utils imgui_backend SDL2 SDL2main)

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

message("Copying shaders to build directory")
file(COPY ${CMAKE_SOURCE_DIR}/src/GraphicsEngine/GLSL DESTINATION ${CMAKE_BINARY_DIR}/application/)

message("Copying kernels to build directory")
file(COPY ${CMAKE_SOURCE_DIR}/src/PhysicsEngine/ocl/kernels DESTINATION ${CMAKE_BINARY_DIR}/application/)

message("Copying SDL lib to build directory")
# Copy SDL2 lib.dll to application build directory
add_custom_command(
        TARGET mainSimulator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:mainSimulator>
)
add_custom_target(CopySDL2Libraries ALL DEPENDS mainSimulator)