//
// Created by charl on 6/9/2023.
//
#pragma once
#include <string>
#include <memory>
#include <glad/gl.h>
#include <SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>

// My libs
#include "Logger.h"
#include "Math.hpp" // Egor Yusov lib


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
        bool initGraphicalWidget();
        bool initPhysicsWidget();
        bool closeWindow();
        void checkMouseState();
        bool checkSDL();
        void displayMainWidget();
        bool popUpMessage(const std::string& title, const std::string& message) const;


        Math::int2 windowSize;

        SDL_Window* window;
        SDL_GLContext OGLContext;

        std::string appName;

        bool init;
    };
}