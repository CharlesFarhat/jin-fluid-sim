//
// Created by charl on 6/12/2023.
//

#include "PositionBasedFluids.h"
#include "ocl/Context.hpp"
#include "Utils.h"
#include "Logger.h"
#include "Geometry.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>


// Define program id
#define PROGRAM_POSITION_BASED_FLUID "PositionBasedFluids"

// utils.cl
#define KERNEL_INFINITE_POS "infPosVerts"
#define KERNEL_RESET_CAMERA_DIST "resetCameraDist"
#define KERNEL_FILL_CAMERA_DIST "fillCameraDist"

// grid.cl
#define KERNEL_RESET_PART_DETECTOR "resetGridDetector"
#define KERNEL_FILL_PART_DETECTOR "fillGridDetector"
#define KERNEL_RESET_CELL_ID "resetCellIDs"
#define KERNEL_FILL_CELL_ID "fillCellIDs"
#define KERNEL_RESET_START_END_CELL "resetStartEndCell"
#define KERNEL_FILL_START_CELL "fillStartCell"
#define KERNEL_FILL_END_CELL "fillEndCell"
#define KERNEL_ADJUST_END_CELL "adjustEndCell"

// fluids.cl
#define KERNEL_RANDOM_POS "randPosVertsFluid"
#define KERNEL_PREDICT_POS "predictPosition"
#define KERNEL_APPLY_BOUNDARY "applyBoundaryCondition"
#define KERNEL_DENSITY "computeDensity"
#define KERNEL_CONSTRAINT_FACTOR "computeConstraintFactor"
#define KERNEL_CONSTRAINT_CORRECTION "computeConstraintCorrection"
#define KERNEL_CORRECT_POS "correctPosition"
#define KERNEL_UPDATE_VEL "updateVel"
#define KERNEL_COMPUTE_VORTICITY "computeVorticity"
#define KERNEL_VORTICITY_CONFINEMENT "applyVorticityConfinement"
#define KERNEL_XSPH_VISCOSITY "applyXsphViscosityCorrection"
#define KERNEL_UPDATE_POS "updatePosition"
#define KERNEL_FILL_COLOR "fillFluidColor"


namespace Physics {
    struct FluidKernelInputs {
        cl_float effectRadius = 0.3f;
        cl_float restDensity = 450.0f;
        cl_float relaxCFM = 600.0f;
        cl_float timeStep = 0.010f;
        cl_uint dim = 3;
        // Artifical pressure if enabled will try to reduce tensile instability
        cl_uint isArtPressureEnabled = 1;
        cl_float artPressureRadius = 0.006f;
        cl_float artPressureCoeff = 0.001f;
        cl_uint artPressureExp = 4;
        // Vorticity confinement if enabled will try to replace lost energy due to virtual damping
        cl_uint isVorticityConfEnabled = 1;
        cl_float vorticityConfCoeff = 0.0004f;
        cl_float xsphViscosityCoeff = 0.0001f;
    };


    PositionBasedFluids::PositionBasedFluids(ModelParams params) : BasePhysicModel(params), simpleMode(true),
                                                                   maxNbPartsInCell(100),
                                                                   nbJacobiIters(2),
                                                                   initalScene(Scenes::Drop),
                                                                   radixSort(params.maxNbParticles),
                                                                   kernelInputs(std::make_unique<FluidKernelInputs>()) {
        // Create the OpenCL simulation program
        createOpenCLProgram();

        // Create associated buffers to send data to GPU
        createOpenCLBuffers();

        // Create OpenCl Kernels inside our cl c file
        createOpenCLKernels();

        init = (kernelInputs != nullptr);

        reset();
    }

    PositionBasedFluids::~PositionBasedFluids() noexcept {};


    bool PositionBasedFluids::createOpenCLProgram() const {
        // Find a GPu and create an openCL contect on it
        CL::Context &clContext = CL::Context::Get();

        float effectRadius = ((float) boxSize) / gridRes;

        std::ostringstream openCLBuildOption;
        openCLBuildOption << "-DEFFECT_RADIUS=" << Utils::FloatToStr(effectRadius);
        openCLBuildOption << " -DABS_WALL_POS=" << Utils::FloatToStr(boxSize / 2.0f);
        openCLBuildOption << " -DGRID_RES=" << gridRes;
        openCLBuildOption << " -DGRID_CELL_SIZE=" << Utils::FloatToStr((float) boxSize / gridRes);
        openCLBuildOption << " -DGRID_NUM_CELLS=" << nbCells;
        openCLBuildOption << " -DNUM_MAX_PARTS_IN_CELL=" << maxNbPartsInCell;
        openCLBuildOption << " -DPOLY6_COEFF="
                          << Utils::FloatToStr(315.0f / (64.0f * Math::PI_F * std::pow(effectRadius, 9.f)));
        openCLBuildOption << " -DSPIKY_COEFF=" << Utils::FloatToStr(15.0f / (Math::PI_F * std::pow(effectRadius, 6.f)));
        openCLBuildOption << " -DMAX_VEL=" << Utils::FloatToStr(30.0f);

        LOG_INFO(openCLBuildOption.str());
        clContext.createProgram(PROGRAM_POSITION_BASED_FLUID,
                                std::vector<std::string>({"fluids.cl", "utils.cl", "grid.cl"}),
                                openCLBuildOption.str());

        LOG_INFO("OpenCL program sucessfully created !");

        return true;
    }

    bool PositionBasedFluids::createOpenCLBuffers() const {
        LOG_INFO("Creating OpenCL Buffers");
        CL::Context &clContext = CL::Context::Get();

        // We are using openGL buffers to create OpenCL Workflow OpenGL buffers <--> OpenCL buffers
        clContext.createGLBuffer("u_cameraPos", cameraVBO, CL_MEM_READ_ONLY);
        clContext.createGLBuffer("p_pos", particlePosVBO, CL_MEM_READ_WRITE);
        clContext.createGLBuffer("p_col", particleColVBO, CL_MEM_READ_WRITE);

        clContext.createGLBuffer("c_partDetector", gridVBO, CL_MEM_READ_WRITE); // TODO: This call abort ...


        clContext.createBuffer("p_density", maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_predPos", 4 * maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_corrPos", 4 * maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_constFactor", maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_vel", 4 * maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_velInViscosity", 4 * maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_vort", 4 * maxNbParticles * sizeof(float), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_cellID", maxNbParticles * sizeof(unsigned int), CL_MEM_READ_WRITE);
        clContext.createBuffer("p_cameraDist", maxNbParticles * sizeof(unsigned int), CL_MEM_READ_WRITE);

        clContext.createBuffer("c_startEndPartID", 2 * nbCells * sizeof(unsigned int), CL_MEM_READ_WRITE);

        LOG_INFO("OpenCL Buffers have been created properly");
        return true;
    }

    bool PositionBasedFluids::createOpenCLKernels() {

        CL::Context &clContext = CL::Context::Get();

        // Init only
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_INFINITE_POS, {"p_pos"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_RANDOM_POS, {"", "p_pos", "p_vel"});

        // For rendering purpose only
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_RESET_PART_DETECTOR, {"c_partDetector"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_FILL_PART_DETECTOR, {"p_pos", "c_partDetector"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_RESET_CAMERA_DIST, {"p_cameraDist"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_FILL_CAMERA_DIST,
                               {"p_pos", "u_cameraPos", "p_cameraDist"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_FILL_COLOR, {"p_density", "", "p_col"});

        // Radix Sort based on 3D grid, using predicted positions, not corrected ones
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_RESET_CELL_ID, {"p_cellID"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_FILL_CELL_ID, {"p_predPos", "p_cellID"});

        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_RESET_START_END_CELL, {"c_startEndPartID"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_FILL_START_CELL, {"p_cellID", "c_startEndPartID"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_FILL_END_CELL, {"p_cellID", "c_startEndPartID"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_ADJUST_END_CELL, {"c_startEndPartID"});

        // Position Based Fluids
        /// Position prediction
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_PREDICT_POS, {"p_pos", "p_vel", "", "p_predPos"});
        /// Boundary conditions
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_APPLY_BOUNDARY, {"p_predPos"});
        /// Jacobi solver to correct position
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_DENSITY,
                               {"p_predPos", "c_startEndPartID", "", "p_density"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_CONSTRAINT_FACTOR,
                               {"p_predPos", "p_density", "c_startEndPartID", "", "p_constFactor"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_CONSTRAINT_CORRECTION,
                               {"p_constFactor", "c_startEndPartID", "p_predPos", "", "p_corrPos"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_CORRECT_POS, {"p_corrPos", "p_predPos"});
        /// Velocity update and correction using vorticity confinement and xsph viscosity
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_UPDATE_VEL, {"p_predPos", "p_pos", "", "p_vel"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_COMPUTE_VORTICITY,
                               {"p_predPos", "c_startEndPartID", "p_vel", "", "p_vort"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_VORTICITY_CONFINEMENT,
                               {"p_predPos", "c_startEndPartID", "p_vort", "", "p_vel"});
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_XSPH_VISCOSITY,
                               {"p_predPos", "c_startEndPartID", "p_velInViscosity", "", "p_vel"});
        /// Position update
        clContext.createKernel(PROGRAM_POSITION_BASED_FLUID, KERNEL_UPDATE_POS, {"p_predPos", "p_pos"});

        LOG_INFO("Properly initiated OpenCl kernels");
        return true;
    }

    void PositionBasedFluids::reset() {
        if (!init) {
            LOG_ERROR("Not initialize correcly !");
            return;
        }

        CL::Context &clContext = CL::Context::Get();

        updatePramsInKernel();

        initSceneParticules();

        clContext.acquireGLBuffers({ "p_pos", "c_partDetector" });
        clContext.runKernel(KERNEL_RESET_PART_DETECTOR, nbCells);
        clContext.runKernel(KERNEL_FILL_PART_DETECTOR, currNbParticles);
        clContext.releaseGLBuffers({ "p_pos", "c_partDetector" });

        clContext.runKernel(KERNEL_RESET_CELL_ID, maxNbParticles);
        clContext.runKernel(KERNEL_RESET_CAMERA_DIST, maxNbParticles);

    }

    void PositionBasedFluids::updatePramsInKernel() {
        if (!init) {
            LOG_ERROR("Not initialize correcly !");
            return;
        }

        // Get OpenCL context
        CL::Context &clContext = CL::Context::Get();

        kernelInputs->dim = 3;

        const float effectRadius = ((float) boxSize) / gridRes;
        kernelInputs->effectRadius = effectRadius;

        // Set kernels args
        clContext.setKernelArg(KERNEL_RANDOM_POS, 0, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_PREDICT_POS, 2, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_UPDATE_VEL, 2, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_DENSITY, 2, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_CONSTRAINT_FACTOR, 3, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_CONSTRAINT_CORRECTION, 3, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_FILL_COLOR, 1, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_COMPUTE_VORTICITY, 3, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_VORTICITY_CONFINEMENT, 3, sizeof(FluidKernelInputs), kernelInputs.get());
        clContext.setKernelArg(KERNEL_XSPH_VISCOSITY, 3, sizeof(FluidKernelInputs), kernelInputs.get());
    }

    // Initialize the Scnene : this is where the magic happend !
    void PositionBasedFluids::initSceneParticules() {

        CL::Context &clContext = CL::Context::Get();

        clContext.acquireGLBuffers({"p_pos", "p_col"});

        std::vector<Math::float3> gridVerts;

        Math::float3 startFluidPos = {0.0f, 0.0f, 0.0f};
        Math::float3 endFluidPos = {0.0f, 0.0f, 0.0f};

        // 3D Geometry used to hold particules volume
        Geometry::Shape3D shape = Geometry::Shape3D::Box;

        switch (initalScene) {
            case Scenes::Bath:
                currNbParticles = Utils::NbParticles::P130K;
                shape = Geometry::Shape3D::Box;
                startFluidPos = {boxSize / -2.0f, boxSize / -2.0f, boxSize / -2.0f};
                endFluidPos = {boxSize / 2.0f, 0.0f, 0.0f};
                break;
            case Scenes::Explosion:
                currNbParticles = Utils::NbParticles::P65K;
                shape = Geometry::Shape3D::Sphere;
                startFluidPos = {boxSize / -6.0f, boxSize / -6.0f, boxSize / -6.0f};
                endFluidPos = {boxSize / 6.0f, boxSize / 6.0f, boxSize / 6.0f};
                break;
            case Scenes::Drop:
                currNbParticles = Utils::NbParticles::P4K;
                shape = Geometry::Shape3D::Box;
                startFluidPos = {boxSize / -10.0f, 2.0f * boxSize / 10.0f, boxSize / -10.0f};
                endFluidPos = {boxSize / 10.0f, 4.0f * boxSize / 10.0f, boxSize / 10.0f};
                break;
            default:
                LOG_ERROR("Unkown case type");
                break;
        }

        const auto &subdiv3D = Utils::GetNbParticlesSubdiv3D((Utils::NbParticles) currNbParticles);
        Math::int3 grid3DRes = {subdiv3D[0], subdiv3D[1], subdiv3D[2]};

        gridVerts = Geometry::Generate3DGrid(shape, grid3DRes, startFluidPos, endFluidPos);

        // Specific case, it adds the sphere to drop
        if (initalScene == Scenes::Drop) {
            currNbParticles += Utils::NbParticles::P65K;
            Math::int3 grid3DRes = {64, 16, 64};
            startFluidPos = {boxSize / -2.0f, boxSize / -2.0f, boxSize / -2.0f};
            endFluidPos = {boxSize / 2.0f, boxSize / -2.55f, boxSize / 2.0f};

            auto bottomGridVerts = Geometry::Generate3DGrid(Geometry::Shape3D::Box, grid3DRes, startFluidPos,
                                                            endFluidPos);

            gridVerts.insert(gridVerts.end(), bottomGridVerts.begin(), bottomGridVerts.end());
        }


        float inf = std::numeric_limits<float>::infinity();
        std::vector<std::array<float, 4>> pos(maxNbParticles, std::array<float, 4>({ inf, inf, inf, 0.0f }));

        std::transform(gridVerts.cbegin(), gridVerts.cend(), pos.begin(),
                       [](const Math::float3& vertPos) -> std::array<float, 4> { return { vertPos.x, vertPos.y, vertPos.z, 0.0f }; });

        clContext.loadBufferFromHost("p_pos", 0, 4 * sizeof(float) * pos.size(), pos.data());

        std::vector<std::array<float, 4>> vel(maxNbParticles, std::array<float, 4>({ 0.0f, 0.0f, 0.0f, 0.0f }));
        clContext.loadBufferFromHost("p_vel", 0, 4 * sizeof(float) * vel.size(), vel.data());

        std::vector<std::array<float, 4>> col(maxNbParticles, std::array<float, 4>({ 0.0f, 0.1f, 1.0f, 0.0f }));
        clContext.loadBufferFromHost("p_col", 0, 4 * sizeof(float) * col.size(), col.data());

        clContext.releaseGLBuffers({ "p_pos", "p_col" });
    }

    void PositionBasedFluids::update() {

    }

}