//
// Created by charl on 6/11/2023.
//
#pragma once

#include "Math.hpp"
#include <glad/gl.h>
#include <memory>
#include <array>

// Lib includes
#include "Shader.h"
#include "Camera.h"

namespace Render {

    enum class UserAction {
        TRANSLATION,
        ROTATION,
        ZOOM
    };

    struct EngineParams {
        size_t currNbParticles = 0;
        size_t maxNbParticles = 0;
        size_t boxSize = 0;
        size_t gridRes = 0;
        size_t pointSize = 4;
        float aspectRatio = 0.0f;
    };

    class GraphicsEngine {
    public:
        explicit GraphicsEngine(struct EngineParams params);

        ~GraphicsEngine();

        // Getter
        [[nodiscard]] inline Math::float3 cameraPos() const {
            return camera ? camera->cameraPos() : Math::float3(0.0f, 0.0f, 0.0f);
        }

        [[nodiscard]] inline Math::float3 focusPos() const {
            return camera ? camera->focusPos() : Math::float3(0.0f, 0.0f, 0.0f);
        }

        [[nodiscard]] inline bool isCameraAutoRotating() const { return camera && camera->isAutoRotating(); }

        void checkMouseEvents(UserAction action, Math::float2 mouseDisplacement);

        //Setters
        inline void autoRotateCamera(bool autoRotate) {
            if (camera)
                camera->enableAutoRotating(autoRotate);
        }

        inline void resetCamera() {
            if (camera)
                camera->reset();
        }

        inline void setWindowSize(Math::int2 windowSize) {
            if (camera)
                camera->setSceneAspectRatio((float) windowSize.x / windowSize.y);
        }

        [[nodiscard]] inline GLuint getPointCloudCoordVBO() const { return pointCloudCoordVBO; }
        [[nodiscard]] inline GLuint getPointCloudColorVBO() const { return pointCloudColorVBO; }
        [[nodiscard]] inline GLuint getCameraCoordVBO() const { return cameraVBO; }
        [[nodiscard]] inline GLuint getGridDetectorVBO() const { return gridDetectorVBO; }

    private:
        // Graphical Pipeline : build shaders --> compute on GPU
        void buildShaders();

        // Init all rendering components
        void initCamera(float sceneAspectRation);

        void loadCameraPosition();

        void initPointCloud();

        void initBox();

        void initGrid();

        void initTarget();


        // Simulation params
        size_t nbMaxParticules;
        size_t boxSize;
        size_t gridResolution;
        size_t nbParticules;
        size_t pointSize;

        // Viz params
        bool isBoxVisible;
        bool isTargetVisible;

        Math::float3 targetPos;

        // Graphical Object
        std::unique_ptr<Camera> camera;
        const GLuint pointCloudPosAttribIndex{0};
        const GLuint pointCloudColAttribIndex{1};
        const GLuint boxPosAttribIndex{2};
        const GLuint targetPosAttribIndex{5};
        const GLuint gridPosAttribIndex { 3 };
        const GLuint gridDetectorAttribIndex { 4 };

        // Shader for pointCloud, box and grid
        std::unique_ptr<Shader> pointCloudShader;
        std::unique_ptr<Shader> boxShader;
        std::unique_ptr<Shader> gridShader;
        std::unique_ptr<Shader> targetShader;

        // OpenGl backend related:
        GLuint VAO;
        GLuint boxVBO;
        GLuint boxEBO;
        GLuint cameraVBO;
        GLuint pointCloudCoordVBO;
        GLuint pointCloudColorVBO;
        GLuint targetVBO;
        GLuint gridDetectorVBO; // ATTENTION : not implemented because of no grid init !
        GLuint gridEBO;
        GLuint gridPosVBO;


        typedef std::array<float, 3> Vertex;
        // Cube geometry for rendering
        const std::array<Vertex, 8> refCubeVertices{
                Vertex({1.f, -1.f, -1.f}),
                Vertex({1.f, 1.f, -1.f}),
                Vertex({-1.f, 1.f, -1.f}),
                Vertex({-1.f, -1.f, -1.f}),
                Vertex({1.f, -1.f, 1.f}),
                Vertex({1.f, 1.f, 1.f}),
                Vertex({-1.f, 1.f, 1.f}),
                Vertex({-1.f, -1.f, 1.f})
        };
        const std::array<GLuint, 24> refCubeIndices{
                0, 1,
                1, 2,
                2, 3,
                3, 0,
                4, 5,
                5, 6,
                6, 7,
                7, 4,
                0, 4,
                1, 5,
                2, 6,
                3, 7
        };
    };
}


