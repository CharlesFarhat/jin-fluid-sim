//
// Created by charl on 6/12/2023.
//

#include "PhysicsControls.h"

UI::PhysicsControls::PhysicsControls(Physics::BasePhysicModel *physicsEngine) : physicsEngine(physicsEngine) {}

void UI::PhysicsControls::display() {

    // cast physicEngine to correct subclass
    auto* positionBasedFluidSim = dynamic_cast<Physics::PositionBasedFluids*>(physicsEngine);

    // Default ImguiPosition
    ImGui::Begin("Physic control widget", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::PushItemWidth(150);

    // Selection of default setup
    auto sceneType = positionBasedFluidSim->getInitialScene();

    // Draw the selected scene name in selector
    const auto& selectSceneName = (Physics::ALL_FLUID_CASES.contains(sceneType))
            ? Physics::ALL_FLUID_CASES.find(sceneType)->second
            : Physics::ALL_FLUID_CASES.cbegin()->second;

    if (ImGui::BeginCombo("Scene", selectSceneName.c_str()))
    {
        for (const auto& sceneT : Physics::ALL_FLUID_CASES)
        {
            if (ImGui::Selectable(sceneT.second.c_str(), sceneType == sceneT.first))
            {
                sceneType = sceneT.first;

                positionBasedFluidSim->setInitialScene(sceneType);
                positionBasedFluidSim->reset();

                LOG_DEBUG("Fluids initial scene correctly switched to {}", Physics::ALL_FLUID_CASES.find(sceneType)->second);
            }
        }
        ImGui::EndCombo();
    }

    ImGui::Value("Number of Particles", (int)positionBasedFluidSim->nbParticles());

    ImGui::Spacing();
    ImGui::Text("Fluid system parameters");
    ImGui::Spacing();


    auto nbJacobiIters = (int)positionBasedFluidSim->getNbJacobiIters();
    if (ImGui::SliderInt("Nb Jacobi Iterations", &nbJacobiIters, 1, 6))
    {
        positionBasedFluidSim->setNbJacobiIters((size_t)nbJacobiIters);
    }

    bool isArtPressureEnabled = positionBasedFluidSim->isArtPressureEnabled();
    if (ImGui::Checkbox("Enable Artificial Pressure", &isArtPressureEnabled))
    {
        positionBasedFluidSim->enableArtPressure(isArtPressureEnabled);
    }

    bool isVorticityConfinementEnabled = positionBasedFluidSim->isVorticityConfinementEnabled();
    if (ImGui::Checkbox("Enable Vorticity Confinement", &isVorticityConfinementEnabled))
    {
        positionBasedFluidSim->enableVorticityConfinement(isVorticityConfinementEnabled);
    }

    ImGui::End();
}
