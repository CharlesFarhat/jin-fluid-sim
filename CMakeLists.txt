cmake_minimum_required(VERSION 3.24) # This is because of OVERRIDE_CMAKE in fetch content declare

# include cmake plugins
include(FetchContent)


project(FluidSimulation VERSION 0.1.0 LANGUAGES CXX)

# CMake/C++ options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS FALSE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(WIN32)
  # SonarLint support for CMake requires the following flag to be set on.
  #(see https://community.sonarsource.com/t/sonarlint-for-visual-studio-v4-38-support-cmake-projects/50127)
  set (CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

###########################################
###### All external libs needed ###########
###########################################
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.10.0
        OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
        OpenCLHeaders
        GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-Headers.git
        GIT_TAG v2022.09.30
        OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(OpenCLHeaders)

FetchContent_Declare(
        OpenCLICDLoader
        GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-ICD-Loader.git
        GIT_TAG v2022.09.30
        OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(OpenCLICDLoader)
set(FETCHCONTENT_QUIET FALSE)
# sdl2
FetchContent_Declare(
        SDL2
        GIT_REPOSITORY  https://github.com/libsdl-org/SDL.git
        GIT_TAG         release-2.24.0
        GIT_PROGRESS    TRUE
)
FetchContent_MakeAvailable(SDL2)
include_directories(${SDL2_SOURCE_DIR}/include)

# import imgui and glad
FetchContent_Declare(
        imgui-glad-glm
        GIT_REPOSITORY https://github.com/cmmw/imgui-glfw-glad-glm.git
        GIT_TAG v4.0.0
)
FetchContent_MakeAvailable(imgui-glad-glm)


###########################################
###### Include our project src ############
###########################################
add_subdirectory(application)
add_subdirectory(src)
