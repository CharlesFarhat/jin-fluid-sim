//
// Created by charl on 6/11/2023.
//
#include "GraphicsControls.h"
#include "Logger.h"

#include <imgui.h>

UI::GraphicsControls::GraphicsControls(Render::GraphicsEngine *graphicsEngine) : graphicsEngine1(graphicsEngine) {}

void UI::GraphicsControls::display() {
    if (!graphicsEngine1) {
        LOG_ERROR("No graphical engine found !");
        return;
    }

    // Default position for Imgui
    ImGui::SetNextWindowPos(ImVec2(15, 192), ImGuiCond_FirstUseEver);

    ImGui::Begin("Graphics Engine Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushItemWidth(150);

    ImGui::Spacing();

    bool isAutoRotating = graphicsEngine1->isCameraAutoRotating();
    if (ImGui::Checkbox(" Auto rotation ", &isAutoRotating))
    {
        graphicsEngine1->autoRotateCamera(isAutoRotating);
    }

    if (ImGui::Button(" Reset Camera "))
    {
        graphicsEngine1->resetCamera();
    }
    ImGui::End();
}

