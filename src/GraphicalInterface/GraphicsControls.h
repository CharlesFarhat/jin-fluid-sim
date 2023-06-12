//
// Created by charl on 6/11/2023.
//
#pragma once
#include <GraphicsEngine.h>

namespace UI {

    class GraphicsControls {
    public:
        explicit GraphicsControls(Render::GraphicsEngine* graphicsEngine);
        virtual ~GraphicsControls() = default;

        void display();

    private:
        Render::GraphicsEngine* graphicsEngine1;
    };
}