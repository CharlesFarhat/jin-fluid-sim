//
// Created by charl on 6/11/2023.
//

// In this file we create the grphical pipeline, see here
// for a overview of steps: https://graphicscompendium.com/opengl/09-opengl-intro


#include <array>
#include "GraphicsEngine.h"

Render::GraphicsEngine::GraphicsEngine(Render::EngineParams params) : nbMaxParticules(params.maxNbParticles),
                                                                      boxSize(params.boxSize),
                                                                      gridResolution(params.gridRes),
                                                                      nbParticules(params.currNbParticles),
                                                                      pointSize(params.pointSize),
                                                                      isBoxVisible(true),
                                                                      targetPos({0.0f, 0.0f, 0.0f}) {
    // We start by init all GL system, please see here (not my code) : https://learnopengl.com/Advanced-OpenGL/Depth-testing
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POINT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glEnable(GL_MULTISAMPLE);

    // We then build the shaders used on GPU
    buildShaders();

    // Vertex Array Object init : it stores Vertex Attribute Pointers, Vertex Buffer Objects, Element Buffer Objects.
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    initCamera(params.aspectRatio);

    initPointCloud();

    initBox();

    initTarget();
}

Render::GraphicsEngine::~GraphicsEngine() {
    glDeleteBuffers(1, &pointCloudCoordVBO);
    glDeleteBuffers(1, &pointCloudColorVBO);
    glDeleteBuffers(1, &boxVBO);
    glDeleteBuffers(1, &cameraVBO);
    glDeleteBuffers(1, &targetVBO);
}

void Render::GraphicsEngine::buildShaders() {
    pointCloudShader = std::make_unique<Shader>("./GLSL/PointCloudFragShader.glsl", "./GLSL/PointCloudVertShader.glsl");
    boxShader = std::make_unique<Shader>("./GLSL/BoxVertShader.glsl", "./GLSL/FragShader.glsl");
    gridShader = std::make_unique<Shader>("./GLSL/GridVertShader.glsl", "./GLSL/FragShader.glsl");
    targetShader = std::make_unique<Shader>("./GLSL/TargetVertShader.glsl", "./GLSL/FragShader.glsl");
}

void Render::GraphicsEngine::initCamera(float sceneAspectRation) {
    camera = std::make_unique<Camera>(sceneAspectRation);

    // Create camera associated VBO
    // Filled at each frame, for OpenCL use
    glGenBuffers(1, &cameraVBO);
    loadCameraPosition();
}

// Load camera position and add object in view to GL_ARRAY_BUFFER for future rendering
void Render::GraphicsEngine::loadCameraPosition() {
    if (!camera)
        return;

    if (camera->isAutoRotating()) {
        const auto angle = Math::float2(0.5f, 0.0f) * Math::PI_F / 180.0f * 0.5f;
        camera->rotate(angle.y, angle.x);
    }

    auto position = camera->cameraPos();
    const std::array<float, 3> cameraCoord = {position[0], position[1], position[2]};
    glBindBuffer(GL_ARRAY_BUFFER, cameraVBO);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &cameraCoord, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Render::GraphicsEngine::initBox() {
    std::array<Vertex, 8> boxVertices = refCubeVertices;
    for (auto &vertex: boxVertices) {
        float x = vertex[0] * boxSize / 2.0f;
        float y = vertex[1] * boxSize / 2.0f;
        float z = vertex[2] * boxSize / 2.0f;
        vertex = {x, y, z};
    }

    glGenBuffers(1, &boxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glVertexAttribPointer(boxPosAttribIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(boxPosAttribIndex);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices.front()) * boxVertices.size(), boxVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &boxEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(refCubeIndices), refCubeIndices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Render::GraphicsEngine::initPointCloud() {
    // Filled by OpenCL
    glGenBuffers(1, &pointCloudCoordVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointCloudCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * nbMaxParticules * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(pointCloudPosAttribIndex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(pointCloudPosAttribIndex);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Filled by OpenCL
    glGenBuffers(1, &pointCloudColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointCloudColorVBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * nbMaxParticules * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(pointCloudColAttribIndex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glEnableVertexAttribArray(pointCloudColAttribIndex);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Render::GraphicsEngine::initTarget() {
    const std::array<float, 3> targetCoord = {targetPos[0], targetPos[1], targetPos[2]};
    glGenBuffers(1, &targetVBO);
    glBindBuffer(GL_ARRAY_BUFFER, targetVBO);
    glVertexAttribPointer(targetPosAttribIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(targetPosAttribIndex);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), &targetCoord, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
