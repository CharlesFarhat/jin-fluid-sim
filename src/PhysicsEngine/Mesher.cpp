//
// Created by charl on 6/16/2023.
//

#include "Mesher.h"
#include <sstream>

Physics::Mesher::Mesher(size_t TSDFGridRes) : TSDFGridRes(TSDFGridRes) {}

std::string Physics::Mesher::addOpenCLBuildOptions() {

    std::ostringstream mesherBuildOptions;
    mesherBuildOptions << " -DTSDF_GRID_RES=" << TSDFGridRes;
    mesherBuildOptions << " -DTSDF_GRID_CELL_SIZE" << nbTSDFGridCells;

    return mesherBuildOptions.str();
}

