//
// Created by charl on 6/9/2023.
//
#pragma once
#include <string>
#include <memory>
#include <glad/gl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <SDL.h>
#undef main

// My libs
#include "Logger.h"
#include "Math.hpp" // Egor Yusov lib
#include <GraphicsEngine.h>
#include "Params.h"
#include "GraphicsControls.h"
#include "BasePhysicModel.h"
#include "PositionBasedFluids.h"
#include "PhysicsControls.h"

namespace Application {

    class FluidSimulator {

    public:
        // Constructor & destructors
        FluidSimulator();

        ~FluidSimulator();

        // Main loop
        void run();

        // Global class variable
        bool isInit() const { return init; }

    private:

        // functions
        bool initWindow();
        bool initGraphicalEngine();
        bool initPhysicsEngine();
        bool initGraphicsControls();
        bool initPhysicsWidget();
        bool closeWindow();
        void checkMouseState();
        bool checkAppStatus();
        void displayMainWidget();


        Math::int2 windowSize;
        Math::int2 mousePrevPos;


        // Rendering related stuff
        SDL_Window* window;
        SDL_GLContext OGLContext;
        std::unique_ptr<Render::GraphicsEngine> graphicsEngine;
        ImVec4 backGroundColor;

        // Widget related varibales
        std::unique_ptr<UI::GraphicsControls> graphicsControls;
        std::unique_ptr<UI::PhysicsControls> physicsControls;

        // Physics related variables
        std::unique_ptr<Physics::BasePhysicModel> physicsEngine;
        Physics::SimType simType; // Selected model

        std::string appName;

        bool init;
    };
}