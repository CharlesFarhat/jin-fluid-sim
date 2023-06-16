//
// Created by charl on 6/16/2023.
//

#include "Mesher.h"
#include <sstream>
#include "ocl/Context.hpp"
#include "Logger.h"



#define PROGRAM_MESHER "mesher"

// OpenCL kernels

// reset TSDF cellID grid
#define KERNEL_TSDF_RESET_CELL_ID "TSDF_resetCellIDs"
// fill TSDF cellID grid
#define KERNEL_TSDF_FILL_CELL_ID "TSDF_fillCellIDs"
// Related to start and end cellID of particules in a cell
#define KERNEL_TSDF_RESET_START_END_CELL "TSDF_resetStartEndCell"
#define KERNEL_TSDF_FILL_START_CELL "TSDF_fillStartCell"
#define KERNEL_TSDF_FILL_END_CELL "TSDF_fillEndCell"
#define KERNEL_TSDF_ADJUST_END_CELL "TSDF_adjustEndCell"

// Compute TSDF
#define KERNEL_TSDF_COMPUTE "TSDF_computeGrid"

Physics::Mesher::Mesher(size_t TSDFGridRes, size_t nbPoints, size_t domainSize, size_t maxnbParticules, RadixSort* radixSort1)
        : simDomainSize(domainSize),
          init(false),
          nbMaxPartPerCellTSDF(100),
          maxNbParticules(maxnbParticules),
          nbTSDFGridCells(
                  TSDFGridRes * TSDFGridRes *
                  TSDFGridRes),
          TSDFGridRes(TSDFGridRes),
          nbParticules(nbPoints),
          radixSort(radixSort1) {

    // create openCl program
    if (!createOpenCLProgram()) {
        LOG_ERROR("Couldn't create openCL program");
        return;
    }

    // create openCL buffers
    if (!createBuffers()) {
        LOG_ERROR("Couldn't create openCL buffers");
        return;
    }

    // create openCL kernels
    if (!createKernels()) {
        LOG_ERROR("Couldn't create openCL kernels");
        return;
    }

    init = true;
}

bool Physics::Mesher::createOpenCLProgram() const {
    CL::Context &clContext = CL::Context::Get();

    std::ostringstream clBuildOptions;

    // TSDF_GRID_RES            - TSDF grid resolution
    // TSDF_GRID_CELL_SIZE      - TSDF size of a cell
    // TSDF_GRID_NUM_CELLS      - TSDF grid number of cells
    // ABS_WALL_POS             - Absolute position of the walls in x,y,z
    // TSDF_NUM_MAX_PARTS_IN_CELL   - maximum number of particles taking into
    // account in a single cell in simplified mode
    clBuildOptions << " -DTSDF_GRID_RES=" << TSDFGridRes;
    clBuildOptions << " -DTSDF_GRID_CELL_SIZE=" << Utils::FloatToStr((float) simDomainSize / TSDFGridRes);;
    clBuildOptions << " -DTSDF_GRID_NUM_CELLS=" << nbTSDFGridCells;
    clBuildOptions << " -DABS_WALL_POS=" << Utils::FloatToStr((float) simDomainSize / 2.0f);
    clBuildOptions << " -DTSDF_NUM_MAX_PARTS_IN_CELL=" << nbMaxPartPerCellTSDF;

    LOG_INFO(clBuildOptions.str());
    LOG_INFO("Creating OpenCL Program for TSDF program");

    if (!clContext.createProgram(PROGRAM_MESHER, "mesher.cl", clBuildOptions.str())) {
        return false;
    }
    return true;
}

bool Physics::Mesher::createBuffers() const {
    CL::Context &clContext = CL::Context::Get();

    LOG_INFO("Creating OpenCL Buffers for TSDF program");
    // Buffer to hold our TSDF voxel grid (an array of signed float, distance to nearest surface)
    // TODO : check datatype for TSDF grid
    clContext.createBuffer("TSDFGrid", sizeof(float) * nbTSDFGridCells, CL_MEM_READ_WRITE);
    // Buffer to hold a tab with pCellID[ID] = id of the cell in TSDF grid the ID particule is in
    clContext.createBuffer("TSDF_cellID", sizeof(unsigned int) * nbParticules, CL_MEM_READ_WRITE);

    // Hold start and end ID of particule in a cell of the grid, sorted by radix and use later for NN search
    // Also used in TSDF to create mesh
    clContext.createBuffer("TSDF_part_startEndID", 2 * sizeof(unsigned int) * nbTSDFGridCells, CL_MEM_READ_WRITE);

    // Buffer to hold all particules positions
    clContext.createBuffer("TSDF_part_pos_tmp", 4 * sizeof(float) * nbParticules, CL_MEM_READ_WRITE);

    LOG_INFO("OpenCL Buffers have been created properly");
    return true;
}

bool Physics::Mesher::createKernels() {

    CL::Context &clContext = CL::Context::Get();

    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_RESET_CELL_ID, {"TSDF_cellID"});
    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_FILL_CELL_ID, {"TSDF_cellID", "TSDF_part_pos_tmp"});
    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_RESET_START_END_CELL, {"TSDF_part_startEndID"});
    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_FILL_START_CELL, {"TSDF_cellID", "TSDF_part_startEndID"});
    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_FILL_END_CELL, {"TSDF_cellID", "TSDF_part_startEndID"});
    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_ADJUST_END_CELL, {"TSDF_part_startEndID"});
    clContext.createKernel(PROGRAM_MESHER, KERNEL_TSDF_COMPUTE,
                           {"TSDF_cellID", "TSDF_part_startEndID", "TSDF_part_pos_tmp", "TSDFGrid"});
    LOG_INFO("Properly initiated OpenCl kernels");
    return true;
}

void Physics::Mesher::reset() const {
    if (!init) {
        LOG_ERROR("Mesher not initialized");
        return;
    }

    CL::Context &clContext = CL::Context::Get();
    clContext.runKernel(KERNEL_TSDF_RESET_CELL_ID, {maxNbParticules});
}

