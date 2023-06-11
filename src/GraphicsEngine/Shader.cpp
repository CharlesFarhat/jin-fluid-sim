//
// Created by charl on 6/11/2023.
//

#include <fstream>
#include "Shader.h"
#include "Logger.h"

Render::Shader::Shader(const char *vertSrcFile, const char *fragSrcFile) {
    // for creation and compile step please refer to : https://www.khronos.org/opengl/wiki/Shader_Compilation
    programID = glCreateProgram();

    // Read shaders from disk
    std::string vertShaderStr = readFromFile(vertSrcFile);
    std::string fragShaderStr = readFromFile(fragSrcFile);

    // Compile loader shaders
    compileShader(GL_VERTEX_SHADER, vertShaderStr.c_str());
    compileShader(GL_FRAGMENT_SHADER, fragShaderStr.c_str());

    glLinkProgram(programID);

    GLint status;
    if (status == GL_FALSE) {
        LOG_ERROR("Shader could'nt be linked");
        return;
    }
}

Render::Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

void Render::Shader::compileShader(GLenum type, const char *source) const {
    GLint shaderID = glCreateShader(type);

    glShaderSource(shaderID, 1, &source, nullptr);
    glCompileShader(shaderID);

    GLint status;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

    // Added debug
    if (status == GL_FALSE) {
        LOG_ERROR("Failed to create shader");
        return;
    }

    glAttachShader(programID, shaderID);
    glDeleteShader(shaderID);
}

const std::string Render::Shader::readFromFile(const char *pathToFile) {
    std::string content;
    std::ifstream fileStream(pathToFile, std::ios::in);

    if (!fileStream.is_open()) {
        std::string path(pathToFile);
        LOG_ERROR("Could not load shader from : " + path + " file does not exist (you may need to copy files to a folder called GLSL, next to the executable)");
        return "";
    }

    std::string line = "";
    while (!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}
