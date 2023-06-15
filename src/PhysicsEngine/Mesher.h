//
// Created by charl on 6/16/2023.
//
#pragma once

#include <string>

namespace Physics {
    class Mesher {
    public:
        Mesher(size_t TSDFGridRes, size_t nbPoints);

        ~Mesher() = default;

    private:
        bool createOpenCLProgram() const;

        bool createBuffers() const;

        size_t nbTSDFGridCells;
        size_t TSDFGridRes;
        size_t nbPoints;
    };
}