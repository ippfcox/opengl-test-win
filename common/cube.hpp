#pragma once

extern "C"
{
#include "glad/glad.h"
#include "GLFW/glfw3.h"
}

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "shader.hpp"

class Cube
{
public:
    Cube();
    ~Cube();
    void Draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model);

private:
    float *vertices_;
    GLuint VBO_, VAO_;
    Shader shader_;
};