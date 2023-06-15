//
// Created by charl on 6/16/2023.
//

#include "Mesher.h"
#include <sstream>
#include "ocl/Context.hpp"
#include "Logger.h"


#define PROGRAM_MESHER "mesher"

Physics::Mesher::Mesher(size_t TSDFGridRes, size_t nbPoints) : TSDFGridRes(TSDFGridRes), nbPoints(nbPoints) {
    // create openCl program
    if (!createOpenCLProgram()) {
        LOG_ERROR("Couldn't create openCL program");
        return;
    }

    if (!createBuffers()) {
        LOG_ERROR("Couldn't create openCL buffers");
        return;
    }

    // create buffers

}

bool Physics::Mesher::createOpenCLProgram() const {
    CL::Context &clContext = CL::Context::Get();

    std::ostringstream clBuildOptions;
    clBuildOptions << " -DTSDF_GRID_RES=" << TSDFGridRes;
    clBuildOptions << " -DTSDF_GRID_CELL_SIZE" << nbTSDFGridCells;

    if (!clContext.createProgram(PROGRAM_MESHER, "mesher.cl", clBuildOptions.str())) {
        return false;
    }
    return true;
}

bool Physics::Mesher::createBuffers() const {
    CL::Context &clContext = CL::Context::Get();

    // Buffer to hold our TSDF voxel grid (an array of signed float, distance to nearest surface)
    clContext.createBuffer("TSDFGrid", sizeof(float) * nbTSDFGridCells, CL_MEM_READ_WRITE);
    // Buffer to hold a tab with pCellID[ID] = id of the cell in TSDF grid the ID particule is in
    clContext.createBuffer("TSDF_cellID", sizeof(unsigned int) * nbPoints, CL_MEM_READ_WRITE);

    // Hold start and end ID of particule in a cell of the grid, sorted by radix and use later for NN search
    // Also used in TSDF to create mesh
    clContext.createBuffer("TSDF_part_startEndID", 2 * sizeof(unsigned int) * nbTSDFGridCells, CL_MEM_READ_WRITE);


    // Buffer to hold all particules positions
    clContext.createBuffer("TSDF_part_pos_tmp", 4 * sizeof(float) * nbPoints, CL_MEM_READ_WRITE);

    return true;
}

