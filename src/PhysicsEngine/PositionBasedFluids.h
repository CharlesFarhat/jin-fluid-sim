//
// Created by charl on 6/12/2023.
//
#pragma once
#include "BasePhysicModel.h"
#include "utils/RadixSort.hpp"


#include <array>
#include <vector>
#include <memory>


namespace Physics {

    struct FluidKernelInputs;

    enum Scenes {
        Bath = 0,
        Explosion = 1,
        Drop = 2
    };

    struct CompareCaseType {
        bool operator()(const Scenes &caseA, const Scenes &caseB) const {
            return (int) caseA < (int) caseB;
        }
    };

    static const std::map<Scenes, std::string, CompareCaseType> ALL_FLUID_CASES{
            {Scenes::Bath,      "Funny bath"},
            {Scenes::Explosion, "Explosion"},
            {Scenes::Drop,      "Drop"},
    };

    class PositionBasedFluids : public BasePhysicModel {
    public:
        PositionBasedFluids(ModelParams params);

        ~PositionBasedFluids();

        void update() override;

        void reset() override;

        void setInitialScene(Scenes sceneI) { initalScene = sceneI; }

        const Scenes getInitialScene() const { return initalScene; }


    private:
        bool createOpenCLProgram() const;

        bool createOpenCLBuffers() const;

        static bool createOpenCLKernels() ;

        void updatePramsInKernel();

        void initSceneParticules();


        bool simpleMode;
        size_t maxNbPartsInCell;
        size_t nbJacobiIters;
        Scenes initalScene;
        // Utils
        RadixSort radixSort;
        std::unique_ptr<FluidKernelInputs> kernelInputs;
    };
}