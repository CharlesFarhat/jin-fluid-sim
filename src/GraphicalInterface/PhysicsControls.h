//
// Created by charl on 6/12/2023.
//
#pragma once

#include <memory>

#include "PositionBasedFluids.h"
#include "BasePhysicModel.h"

namespace UI {
    class PhysicsControls {
    public:
        explicit PhysicsControls(Physics::BasePhysicModel* physicsEngine);
        virtual ~PhysicsControls() = default;

        void display();

    private:
        Physics::BasePhysicModel* physicsEngine;
    };
}
