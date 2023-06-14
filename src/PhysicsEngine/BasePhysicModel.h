//
// Created by charl on 6/11/2023.
//
#pragma once

#include <string>
#include <map>

#include "Math.hpp"


namespace Physics {

    // Implemented simulation models
    enum SimType {
        POSITION_BASED_FLUIDS = 0,
        FLIP = 1,
        LATTICE_BOLTZMANN = 2
    };

    struct CompareModelType {
        bool operator()(const SimType &modelA, const SimType &modelB) const {
            return (int) modelA < (int) modelB;
        }
    };

    static const std::map<SimType, std::string, CompareModelType> ALL_MODELS{
            {SimType::POSITION_BASED_FLUIDS, "Position based fluid sim"}, // Position Based Fluids by NVIDIA team (Macklin and Muller)
            {SimType::FLIP,                  "FLIP method"}, //  Hybrid method between a particle based and volume based fluid simulations
            {SimType::LATTICE_BOLTZMANN,     "Lattice Boltzmann methods"} // Streaming and collision processes.
    };

    struct ModelParams {
        size_t currNbParticles = 0;
        size_t maxNbParticles = 0;
        size_t boxSize = 0;
        size_t gridRes = 0;
        float velocity = 0.0f;
        unsigned int particlePosVBO = 0;
        unsigned int particleColVBO = 0;
        unsigned int cameraVBO = 0;
        unsigned int gridVBO = 0;
    };

    // This hold the type of limit conditions used for the simulation
    enum class Boundary {
        BouncingWall,
        CyclicWall
    };

    // All model are using OpenCL backend, wanted to use Vulkan but too complex to implement
    class BasePhysicModel {
    public:
        explicit BasePhysicModel(ModelParams params);

        virtual ~BasePhysicModel();

        virtual void update() = 0;

        virtual void reset() = 0;

        // Setters and getters
        [[nodiscard]] size_t getMaxNbParticles() const { return maxNbParticles; }

        void setNbParticles(size_t nbSelParticles) { currNbParticles = nbSelParticles; }

        [[nodiscard]] size_t nbParticles() const { return currNbParticles; }

        void setBoundary(Boundary boundary) {
            boundary = boundary;
        }

        [[nodiscard]] Boundary getBoundary() const { return boundary; }

        [[nodiscard]] bool isInit() const { return init; }

        void setPause(bool pause) { pause = pause; }

        [[nodiscard]] bool isPause() const { return pause; }

        virtual void setVelocity(float velocity) { velocity = velocity; }

        [[nodiscard]] float getVelocity() const { return velocity; }

        [[nodiscard]] bool isProfilingEnabled() const;

        void enableProfiling(bool enable);

        bool isUsingIGPU() const;

    protected:
        bool init;
        bool pause;

        size_t maxNbParticles;
        size_t currNbParticles;

        size_t boxSize;

        size_t gridRes;
        size_t nbCells;

        float velocity;

        Boundary boundary;

        // Used to bridge to graphics
        unsigned int particlePosVBO;
        unsigned int particleColVBO;
        unsigned int cameraVBO;
        unsigned int gridVBO;
    };
}


