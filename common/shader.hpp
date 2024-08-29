#pragma once

extern "C"
{
#include "glad/glad.h"
#include "GLFW/glfw3.h"
}

#include <string>

class Shader
{
public:
    Shader();
    virtual ~Shader();

    int InitBySrc(const std::string &vertex_shader_src, const std::string &fragment_shader_src);
    int InitByFilename(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename);

    void Use();
    void SetUniform(const std::string &uniform_name, int i0);
    void SetUniform(const std::string &uniform_name, float f0);
    void SetUniform(const std::string &uniform_name, float f0, float f1, float f2);
    void SetUniform(const std::string &uniform_name, float f0, float f1, float f2, float f3);
    void SetUniform(const std::string &uniform_name, const float *matrix4fv);

private:
    GLuint vertex_shader_, fragment_shader_, program_;

    int CheckError(GLuint x);
    GLuint CreateShader(GLenum type, const std::string &shader_src);
    void DeleteShader(GLuint shader);
    GLuint LinkProgram(GLuint vertex_shader, GLuint fragment_shader);
    const std::string ReadFile(const std::string &filename);
};
