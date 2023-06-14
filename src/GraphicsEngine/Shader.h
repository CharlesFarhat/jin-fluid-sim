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

        void activate() const;
        static void desactivate();

        // activate shader program before calling these functions
        void setUniform(const std::string& name, bool value) const {};
        void setUniform(const std::string& name, int value) const;
        void setUniform(const std::string& name, float value) const;
        void setUniform(const std::string& name, const Math::float4x4& mat) const;
        void setUniform(const std::string& name, const Math::float3& vec) const;


    private:
        GLint getUniformLocation(const std::string& name) const;

        // Shader Compiler see : https://www.khronos.org/opengl/wiki/Shader_Compilation
        const std::string readFromFile(const char* pathToFile);
        void compileShader(GLenum type, const char* source) const;

        GLint programID;
    };
}


