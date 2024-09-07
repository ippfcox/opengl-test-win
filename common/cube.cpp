#include "cube.hpp"
#include "spdlog/spdlog.h"

Cube::Cube()
{
    vertices_ = new float[]{
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), vertices_, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO_);
    glBindVertexArray(VAO_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    std::string vertex_shader_src =
        "#version 330 core                                                   \n"
        "layout (location = 0) in vec3 aPos;                                 \n"
        "uniform mat4 projection;                                            \n"
        "uniform mat4 view;                                                  \n"
        "uniform mat4 model;                                                 \n"
        "void main()                                                         \n"
        "{                                                                   \n"
        "    gl_Position = projection * view * model *  vec4(aPos, 1.0);     \n"
        "}                                                                   \n"
        "";
    std::string fragment_shader_src =
        "#version 330 core                                                   \n"
        "out vec4 FragColor;                                                 \n"
        "void main()                                                         \n"
        "{                                                                   \n"
        "    FragColor = vec4(1.0, 1.0, 1.0, 1.0);                           \n"
        "}                                                                   \n"
        "";
    int ret = shader_.InitBySrc(vertex_shader_src, fragment_shader_src);
    if (ret != 0)
    {
        SPDLOG_ERROR("InitBySrc failed");
        return;
    }
}

Cube::~Cube()
{
    if (vertices_)
    {
        delete[] vertices_;
        vertices_ = nullptr;
    }
}

void Cube::Draw(glm::mat4 projection, glm::mat4 view, glm::mat4 model)
{
    shader_.Use();
    shader_.SetUniform("projection", projection);
    shader_.SetUniform("view", view);
    shader_.SetUniform("model", model);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
    glUseProgram(GL_NONE);
}