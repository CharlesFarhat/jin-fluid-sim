//
// Created by charl on 6/11/2023.
//
#pragma once

#include "Math.hpp"
#include <glad/gl.h>
#include <string>

namespace Render {
    class Shader {
    public:
        Shader(const char *vertSrcFile, const char *fragSrcFile);

        ~Shader();


    private:
        // Shader Compiler see : https://www.khronos.org/opengl/wiki/Shader_Compilation
        const std::string readFromFile(const char* pathToFile);
        void compileShader(GLenum type, const char* source) const;

        GLint programID;
    };
}


