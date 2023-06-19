//
// Created by charl on 6/12/2023.
//
#pragma once

#include "BasePhysicModel.h"
#include "utils/RadixSort.hpp"
#include "Mesher.h"


#include <array>
#include <vector>
#include <memory>


namespace Physics {

    struct FluidKernelInputs;

    enum Scenes {
        Bath = 0,
        Drop = 2,
        DoubleDrop = 3,
    };

    struct CompareCaseType {
        bool operator()(const Scenes &caseA, const Scenes &caseB) const {
            return (int) caseA < (int) caseB;
        }
    };

    static const std::map<Scenes, std::string, CompareCaseType> ALL_FLUID_CASES{
            {Scenes::Bath,      "Funny bath"},
            {Scenes::Drop,      "Drop"},
            {Scenes::DoubleDrop,"Double drop"},
    };

    class PositionBasedFluids : public BasePhysicModel {
    public:
        PositionBasedFluids(ModelParams params);

        ~PositionBasedFluids();

        void update() override;

        void reset() override;

        void setInitialScene(Scenes sceneI) { initalScene = sceneI; }

        const Scenes getInitialScene() const { return initalScene; }

        // All getter and setters for fluids params

        void setNbJacobiIters(size_t nbIters) {
            if (!init) return;
            nbJacobiIters = nbIters;
        }

        [[nodiscard]] size_t getNbJacobiIters() const { return init ? nbJacobiIters : 0; }
        void enableArtPressure(bool enable);
        [[nodiscard]] bool isArtPressureEnabled() const;
        void enableVorticityConfinement(bool enable);
        [[nodiscard]] bool isVorticityConfinementEnabled() const;



    private:
        bool createOpenCLProgram() const;

        bool createOpenCLBuffers() const;

        static bool createOpenCLKernels();

        void updatePramsInKernel();

        void initSceneParticules();


        bool simpleMode;
        size_t maxNbPartsInCell;
        size_t nbJacobiIters;
        Scenes initalScene;
        // Utils
        std::unique_ptr<RadixSort> radixSort;
        std::unique_ptr<FluidKernelInputs> kernelInputs;
        std::unique_ptr<Mesher> mesher;
    };
}