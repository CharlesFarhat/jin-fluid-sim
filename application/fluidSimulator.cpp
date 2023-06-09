#include "fluidSimulator.h"

// SDl workaround...
#include <SDL.h>
#undef main

// Set OpenGL compiler version
constexpr auto GLSL_VERSION = "#version 130";

namespace Application {

    FluidSimulator::FluidSimulator() : init(false), appName("Realtime Fluid Simulator"), windowSize(1920, 1080) {
        LOG_INFO("Starting a RT physicaly accurate fluid simulator !");

        if (!initWindow()) {
            LOG_ERROR("Failed to init main window");
            exit(0);
        }

        LOG_INFO("System initialize !");
        init = true;

    }

    FluidSimulator::~FluidSimulator() {
        LOG_INFO("Quitting RT fluid simulator");
    }

    bool FluidSimulator::initWindow() {
        // setup SDL with video, controler and time
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER) < 0) {
            LOG_ERROR("Failed to initialize the SDL2 library");
            return false;
        }

        // GL 3.0 + GLSL 130 see : https://github.com/ocornut/imgui/issues/1466
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

        // Create window !
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
        auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI |
                                               SDL_WINDOW_SHOWN);
        window = SDL_CreateWindow(appName.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowSize.x,
                                  windowSize.y, window_flags);
        OGLContext = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, OGLContext);
        SDL_GL_SetSwapInterval(1); // Enable vsync

        // init OpenGL
        bool GL_err = gladLoaderLoadGL() == 0;
        if (GL_err) {
            LOG_ERROR("OpenGL failed to load loader !");
            return false;
        }

        // IMGUI related init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark(); // Change your app style ! (Dark of white)
        ImGui_ImplSDL2_InitForOpenGL(window, OGLContext);
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);

        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        return true;
    }

    void FluidSimulator::run() {
        LOG_INFO("Run !");
    }

}

int main() {
    Application::FluidSimulator ourSimulation;

    if (ourSimulation.isInit()) {
        ourSimulation.run();
    }

    return 0;
}