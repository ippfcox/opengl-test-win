#include <string>
#include <strstream>
#include <vector>
#include <fstream>
#include <sstream>
#include "shader.hpp"
#include "spdlog/spdlog.h"

Shader::Shader()
{
}

Shader::~Shader()
{
}

int Shader::InitBySrc(const std::string &vertex_shader_src, const std::string &fragment_shader_src)
{
    vertex_shader_ = CreateShader(GL_VERTEX_SHADER, vertex_shader_src);
    if (vertex_shader_ == 0)
    {
        SPDLOG_ERROR("CreateShader failed");
        goto error1;
    }

    fragment_shader_ = CreateShader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (fragment_shader_ == 0)
    {
        SPDLOG_ERROR("CreateShader failed");
        goto error2;
    }

    program_ = LinkProgram(vertex_shader_, fragment_shader_);
    if (program_ == 0)
    {
        SPDLOG_ERROR("LinkProgram failed");
        goto error3;
    }

    return 0;

error3:
    DeleteShader(fragment_shader_);
error2:
    DeleteShader(vertex_shader_);
error1:
    return -1;
}

int Shader::InitByFilename(const std::string &vertex_shader_filename, const std::string &fragment_shader_filename)
{
    auto vertex_shader_src = ReadFile(vertex_shader_filename);
    if (vertex_shader_src.length() == 0)
    {
        SPDLOG_ERROR("ReadFile failed");
        return -1;
    }

    auto fragment_shader_src = ReadFile(fragment_shader_filename);
    if (fragment_shader_src.length() == 0)
    {
        SPDLOG_ERROR("ReadFile failed");
        return -1;
    }

    return InitBySrc(vertex_shader_src, fragment_shader_src);
}

void Shader::Use()
{
    glUseProgram(program_);
}

void Shader::SetUniform(const std::string &uniform_name, int i0)
{
    glUniform1i(glGetUniformLocation(program_, uniform_name.c_str()), i0);
}

void Shader::SetUniform(const std::string &uniform_name, float f0)
{
    glUniform1f(glGetUniformLocation(program_, uniform_name.c_str()), f0);
}

void Shader::SetUniform(const std::string &uniform_name, float f0, float f1, float f2)
{
    glUniform3f(glGetUniformLocation(program_, uniform_name.c_str()), f0, f1, f2);
}

void Shader::SetUniform(const std::string &uniform_name, glm::vec3 v)
{
    SetUniform(uniform_name, v.x, v.y, v.z);
}

void Shader::SetUniform(const std::string &uniform_name, float f0, float f1, float f2, float f3)
{
    glUniform4f(glGetUniformLocation(program_, uniform_name.c_str()), f0, f1, f2, f3);
}

void Shader::SetUniform(const std::string &uniform_name, glm::vec4 v)
{
    SetUniform(uniform_name, v.x, v.y, v.z, v.w);
}

void Shader::SetUniform(const std::string &uniform_name, const float *matrix4fv)
{
    glUniformMatrix4fv(glGetUniformLocation(program_, uniform_name.c_str()), 1, GL_FALSE, matrix4fv);
}

void Shader::SetUniform(const std::string &uniform_name, glm::mat4 m)
{
    SetUniform(uniform_name, glm::value_ptr(m));
}

int Shader::CheckError(GLuint x)
{
    PFNGLGETSHADERIVPROC glGetiv;
    PFNGLGETSHADERINFOLOGPROC glGetInfoLog;
    PFNGLDELETEPROGRAMPROC glDelete;
    GLenum pname_status;

    if (glIsShader(x))
    {
        glGetiv = glGetShaderiv;
        glGetInfoLog = glGetShaderInfoLog;
        glDelete = glDeleteShader;
        pname_status = GL_COMPILE_STATUS;
    }
    else if (glIsProgram(x))
    {
        glGetiv = glGetProgramiv;
        glGetInfoLog = glGetProgramInfoLog;
        glDelete = glDeleteProgram;
        pname_status = GL_LINK_STATUS;
    }
    else
    {
        SPDLOG_ERROR("invalid x");
        return -1;
    }

    GLint success;
    glGetiv(x, pname_status, &success);
    if (!success)
    {
        GLint info_len = 0;
        glGetiv(x, GL_INFO_LOG_LENGTH, &info_len);
        if (info_len > 1)
        {
            std::vector<char> info_log(info_len);
            glGetInfoLog(x, info_len, NULL, info_log.data());
            SPDLOG_ERROR("failed: {}", info_log.data());
        }
        glDelete(x);
        return -1;
    }

    return 0;
}

GLuint Shader::CreateShader(GLenum type, const std::string &shader_src)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0)
    {
        SPDLOG_ERROR("glCreateShader failed");
        return 0;
    }

    const char *shader_src_c = shader_src.c_str();

    // load the shader source
    glShaderSource(shader, 1, &shader_src_c, NULL);

    // compile the shader
    glCompileShader(shader);

    if (CheckError(shader) != 0)
    {
        SPDLOG_ERROR("error accured");
        return 0;
    }

    return shader;
}

void Shader::DeleteShader(GLuint shader)
{
    glDeleteShader(shader);
}

GLuint Shader::LinkProgram(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();
    if (program == 0)
    {
        SPDLOG_ERROR("glCreateProgram failed");
        return 0;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    if (CheckError(program) != 0)
    {
        SPDLOG_ERROR("error occured");
        return 0;
    }

    return program;
}

const std::string Shader::ReadFile(const std::string &filename)
{
    std::ifstream shader_file(filename);
    if (!shader_file.is_open())
    {
        SPDLOG_ERROR("open failed");
        return std::string{};
    }

    std::stringstream shader_buffer;
    shader_buffer << shader_file.rdbuf();

    shader_file.close();

    return shader_buffer.str();
}