#include "fluidSimulator.h"

// SDl workaround...
#include <SDL.h>

#undef main

// Set OpenGL compiler version
constexpr auto GLSL_VERSION = "#version 130";

namespace Application {

    FluidSimulator::FluidSimulator() : windowSize(1920, 1080),
                                       simType(Physics::SimType::POSITION_BASED_FLUIDS),
                                       appName("Realtime Fluid Simulator"),
                                       init(false) {
        LOG_INFO("Starting a RT physicaly accurate fluid simulator !");

        if (!initWindow()) {
            LOG_ERROR("Failed to init main window");
            exit(0);
        }

        if (!initGraphicalEngine()) {
            LOG_ERROR("Graphical engine could not be init");
            return;
        }

        if (!initGraphicsControls()) {
            LOG_ERROR("Graphic control widgets not init !");
            return;
        }

        if (!initPhysicsEngine()) {
            LOG_ERROR("Failed to init physics Engine !");
            return;
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
        if (bool GL_err = gladLoaderLoadGL() == 0) {
            LOG_ERROR("OpenGL failed to load loader !");
            return false;
        }

        // IMGUI related init
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark(); // Change your app style ! (Dark of white)
        ImGui_ImplSDL2_InitForOpenGL(window, OGLContext);
        ImGui_ImplOpenGL3_Init(GLSL_VERSION);

        ImGuiIO &io = ImGui::GetIO();
        (void) io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        return true;
    }

    bool FluidSimulator::initGraphicalEngine() {
        // Create render engine
        Render::EngineParams params;
        params.maxNbParticles = Utils::ALL_NB_PARTICLES.crbegin()->first;
        params.boxSize = Utils::BOX_SIZE;
        params.gridRes = Utils::GRID_RES;
        params.aspectRatio = (float) windowSize.x / (float) windowSize.y;

        graphicsEngine = std::make_unique<Render::GraphicsEngine>(params);

        if (graphicsEngine != nullptr) {
            return true;
        }
        return false;
    }

    void FluidSimulator::run() {
        LOG_INFO("Run !");
    }

    bool FluidSimulator::initGraphicsControls() {
        // Get is used to retreive the base pointer of graphics engine, we don't have ownership
        graphicsControls = std::make_unique<UI::GraphicsControls>(graphicsEngine.get());

        if (!graphicsControls) {
            LOG_ERROR("Graphics control widget not init");
            return false;
        }
        return true;
    }

    bool FluidSimulator::initPhysicsEngine() {
        Physics::ModelParams params;
        params.maxNbParticles = Utils::ALL_NB_PARTICLES.crbegin()->first;
        params.boxSize = Utils::BOX_SIZE;
        params.gridRes = Utils::GRID_RES;
        params.velocity = 1.0f;
        params.particlePosVBO = (unsigned int) graphicsEngine->getPointCloudCoordVBO();
        params.particleColVBO = (unsigned int) graphicsEngine->getPointCloudColorVBO();
        params.cameraVBO = (unsigned int) graphicsEngine->getCameraCoordVBO();
        params.gridVBO = (unsigned int) graphicsEngine->getGridDetectorVBO();

        if (physicsEngine) {
            LOG_DEBUG("Physics engine already existing, resetting it");
            physicsEngine.reset();
        }

        switch ((int) simType) {
            case Physics::SimType::POSITION_BASED_FLUIDS:
                physicsEngine = std::make_unique<Physics::PositionBasedFluids>(params);
                break;
            case Physics::SimType::LATTICE_BOLTZMANN:
                LOG_INFO("Not implemented Yet !");
                break;
            case Physics::SimType::FLIP:
                LOG_INFO("Not implemented Yet !");
                break;
        }

        if (!physicsEngine) {
            LOG_ERROR("Physic engine not runnig !");
            return false;
        }
        return true;
    }

}

int main() {
    Application::FluidSimulator ourSimulation;

    if (ourSimulation.isInit()) {
        ourSimulation.run();
    }

    return 0;
}