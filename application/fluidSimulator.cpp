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
                                       init(false),
                                       backGroundColor(0.0f, 0.0f, 0.0f, 1.00f) {
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

        if (!initPhysicsWidget()) {
            LOG_ERROR("Physic Control system not initialized");
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

    bool FluidSimulator::initPhysicsWidget() {
        physicsControls = std::make_unique<UI::PhysicsControls>(physicsEngine.get());

        if (physicsControls == nullptr) {
            return false;
        }
        return true;
    }

    /*********************************************************************/
    /*********************************************************************/
    //                                                                   //
    //                                                                   //
    //                      RUN MAIN FUNCTION                            //
    //                                                                   //
    //                                                                   //
    /*********************************************************************/
    /*********************************************************************/


    void FluidSimulator::run() {
        LOG_INFO("Start RUN function");

        bool stopRunning = false;
        while (!stopRunning) {
            stopRunning = checkAppStatus();

            checkMouseState();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // Display main control panel
            displayMainWidget();

            // Draw here all widgets
            graphicsControls->display();
            physicsControls->display();


            ImGuiIO &io = ImGui::GetIO();
            glViewport(0, 0, (int) io.DisplaySize.x, (int) io.DisplaySize.y);

            glClearColor(backGroundColor.x, backGroundColor.y, backGroundColor.z, backGroundColor.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Make the simulation happen
            physicsEngine->update();


            ImGui::Render();

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            SDL_GL_SwapWindow(window);

        }

        closeWindow();
    }

    bool FluidSimulator::checkAppStatus() {
        bool stopRunning = false;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_QUIT:
                    stopRunning = true;
                    break;
                case SDL_WINDOWEVENT :
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE &&
                        event.window.windowID == SDL_GetWindowID(window)) {
                        stopRunning = true;
                    }
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        windowSize = Math::int2(event.window.data1, event.window.data2);
                        graphicsEngine->setWindowSize(windowSize);
                    }
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT || event.button.button == SDL_BUTTON_RIGHT) {
                        Math::int2 currentMousePos;
                        SDL_GetMouseState(&currentMousePos.x, &currentMousePos.y);
                        mousePrevPos = currentMousePos;

                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        graphicsEngine->checkMouseEvents(Render::UserAction::ZOOM, Math::float2(-1.2f, 0.f));
                    } else if (event.wheel.y < 0) {
                        graphicsEngine->checkMouseEvents(Render::UserAction::ZOOM, Math::float2(1.2f, 0.f));
                    }
                    break;
                case SDL_KEYDOWN:
                    bool isPaused = physicsEngine->isPause();
                    physicsEngine->setPause(!isPaused);
                    break;
            }
        }
        return stopRunning;
    }

    void FluidSimulator::checkMouseState() {
        ImGuiIO &io = ImGui::GetIO();
        (void) io;

        if (io.WantCaptureMouse)
            return;

        Math::int2 currentMousePos;
        auto mouseState = SDL_GetMouseState(&currentMousePos.x, &currentMousePos.y);
        Math::int2 delta = currentMousePos - mousePrevPos;
        Math::float2 fDelta((float) delta.x, (float) delta.y);

        if (mouseState & SDL_BUTTON(1)) {
            graphicsEngine->checkMouseEvents(Render::UserAction::ROTATION, fDelta);
            mousePrevPos = currentMousePos;
        } else if (mouseState & SDL_BUTTON(3)) {
            graphicsEngine->checkMouseEvents(Render::UserAction::TRANSLATION, fDelta);
            mousePrevPos = currentMousePos;
        }
    }

    void FluidSimulator::displayMainWidget() {
        // First default pos
        ImGui::SetNextWindowPos(ImVec2(15, 12), ImGuiCond_FirstUseEver);

        ImGui::Begin("Main Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::PushItemWidth(150);

        if (!isInit())
            return;

        // Selection of the physical model
        const auto &selModelName = (Physics::ALL_MODELS.find(simType) != Physics::ALL_MODELS.end())
                                   ? Physics::ALL_MODELS.find(simType)->second
                                   : Physics::ALL_MODELS.cbegin()->second;

        if (ImGui::BeginCombo("Physical Model", selModelName.c_str())) {
            for (const auto &model: Physics::ALL_MODELS) {
                if (ImGui::Selectable(model.second.c_str(), simType == model.first)) {
                    simType = model.first;

                    if (!initPhysicsEngine()) {
                        LOG_ERROR("Failed to change physics engine");
                        return;
                    }

                    if (!initPhysicsWidget()) {
                        LOG_ERROR("Failed to change physics widget");
                        return;
                    }

                    LOG_INFO("Application correctly switched to {}", Physics::ALL_MODELS.find(simType)->second);
                }
            }
            ImGui::EndCombo();
        }

        bool isOnPaused = physicsEngine->isPause();
        std::string pauseRun = isOnPaused ? "  Start  " : "  Pause  ";
        if (ImGui::Button(pauseRun.c_str())) {
            physicsEngine->setPause(!isOnPaused);
        }

        ImGui::SameLine();

        if (ImGui::Button("  Reset  ")) {
            physicsEngine->reset();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::End();
    }

    bool FluidSimulator::closeWindow() {
        // Destroying all SDL-OpenGL-ImGUI stuff
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        SDL_GL_DeleteContext(OGLContext);
        SDL_DestroyWindow(window);
        SDL_Quit();

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