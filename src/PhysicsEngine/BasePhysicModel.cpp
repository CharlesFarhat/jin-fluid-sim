//
// Created by charl on 6/11/2023.
//

#include "BasePhysicModel.h"
#include "ocl/Context.hpp"

Physics::BasePhysicModel::BasePhysicModel(ModelParams params) : init(false),
                                                                pause(false),
                                                                maxNbParticles(params.maxNbParticles),
                                                                currNbParticles(params.currNbParticles),
                                                                boxSize(params.boxSize), gridRes(params.gridRes),
                                                                nbCells(params.gridRes * params.gridRes *
                                                                        params.gridRes),
                                                                velocity(params.velocity),
                                                                boundary(Boundary::BouncingWall),
                                                                particlePosVBO(params.particlePosVBO),
                                                                particleColVBO(params.particleColVBO),
                                                                cameraVBO(params.cameraVBO),
                                                                gridVBO(params.gridVBO) {}

Physics::BasePhysicModel::~BasePhysicModel() {
    CL::Context::Get().release();
}

bool Physics::BasePhysicModel::isProfilingEnabled() const {
    CL::Context& clContext = Physics::CL::Context::Get();
    return clContext.isProfiling();
}

void Physics::BasePhysicModel::enableProfiling(bool enable) {
    CL::Context& clContext = Physics::CL::Context::Get();
    clContext.enableProfiler(enable);
}

bool Physics::BasePhysicModel::isUsingIGPU() const {
    const std::string& platformName = Physics::CL::Context::Get().getPlatformName();
    return (platformName.find("Intel") != std::string::npos);
}
