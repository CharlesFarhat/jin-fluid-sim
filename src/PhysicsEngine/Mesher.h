//
// Created by charl on 6/16/2023.
//
#pragma once

#include <string>
#include <memory>
#include "utils/RadixSort.hpp"
#include "Utils.h"

namespace Physics {
    class Mesher {
    public:
        Mesher(size_t TSDFGridRes, size_t nbPqrticules, size_t domainSize, size_t maxnbParticules, RadixSort* radixSort1);

        void reset() const;

        void updateMesher(const std::string& inputPartPos);

        ~Mesher() = default;

    private:
        bool createOpenCLProgram() const;

        bool createBuffers() const;

        static bool createKernels();

        size_t simDomainSize;

        bool init;
        size_t nbMaxPartPerCellTSDF;
        size_t maxNbParticules;
        size_t nbTSDFGridCells;
        size_t TSDFGridRes;
        size_t nbParticules;

        //Sort system
        RadixSort* radixSort;
    };
}