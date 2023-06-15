//
// Created by charl on 6/16/2023.
//
#pragma once

#include <string>

namespace Physics {
    class Mesher {
        Mesher(size_t TSDFGridRes);
        ~Mesher() = default;

        std::string addOpenCLBuildOptions();

    private:
        size_t nbTSDFGridCells;
        size_t TSDFGridRes;
    };
}