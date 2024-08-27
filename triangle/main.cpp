#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "spdlog/spdlog.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

int main()
{
    spdlog::set_level(spdlog::level::debug);

    ////////////////////////////////////////////////////////////////////////////
    //                              glfw                                      //
    ////////////////////////////////////////////////////////////////////////////

    // glfw init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create window
    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
    if (!window)
    {
        SPDLOG_ERROR("glfwCreateWindow failed");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    ////////////////////////////////////////////////////////////////////////////
    //                                glad                                    //
    ////////////////////////////////////////////////////////////////////////////

    // glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        SPDLOG_ERROR("gladLoadGLLoader failed");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////
    //                       VBO/VAO/EBO                                      //
    ////////////////////////////////////////////////////////////////////////////

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); // bind VAO, below vertex attrib will be stored to VAO

    // vertex:             texture:
    //       1.0                1.0
    //        |                  |
    // -1.0 --+-- 1.0      0.0 --+-- 1.0
    //        |                  |
    //      -1.0                0.0

    // vertex.x, vertex.y, vertex.z, texture.x, texture.y
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)0); // 3 means 3 number in a row
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int indices[36];
    for (int i = 0; i < 36; ++i)
        indices[i] = i;
    GLuint EBO; // store use order indices for vertices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ////////////////////////////////////////////////////////////////////////////
    //                            shader                                      //
    ////////////////////////////////////////////////////////////////////////////

    Shader shader0;
    int ret = shader0.InitByFilename("../../triangle/vertex.glsl", "../../triangle/fragment.glsl");
    if (ret != 0)
    {
        SPDLOG_ERROR("InitByFilename failed");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////
    //                                 texture                                //
    ////////////////////////////////////////////////////////////////////////////

    int texture_width, texture_height, texture_channels;
    stbi_uc *data = stbi_load("../../asset/container.jpg", &texture_width, &texture_height, &texture_channels, 0);
    if (!data)
    {
        SPDLOG_ERROR("stbi_load failed");
        return -1;
    }
    GLenum texture_format = texture_channels == 3 ? GL_RGB : GL_RGBA;

    GLuint texture_0;
    glGenTextures(1, &texture_0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, texture_width, texture_height, 0, texture_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(texture_0);
    stbi_image_free(data);

    shader0.Use();
    shader0.SetUniform("aTexture0", 0); // sampler2D aTexture <-> GL_TEXTURE0
    glUseProgram(GL_NONE);

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); });

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);

    ////////////////////////////////////////////////////////////////////////////
    //                           matrix                                       //
    ////////////////////////////////////////////////////////////////////////////
    glm::mat4 model(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 view(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    glm::mat4 projection(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

    glm::vec3 cube_positions[] = {
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    ////////////////////////////////////////////////////////////////////////////
    //                            glfw loop                                   //
    ////////////////////////////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window))
    {
        // process input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader0.Use();
        glBindVertexArray(VAO); // bind VAO, vertex config is ready, EBO is binded automatically

        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(0.01f), glm::vec3(0.5f, 1.0f, 0.0f));

        for (int i = 0; i < sizeof(cube_positions) / sizeof(glm::vec3); ++i)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, cube_positions[i]);
            model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
            shader0.SetUniform("model", glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        }
        shader0.SetUniform("model", glm::value_ptr(model));
        shader0.SetUniform("view", glm::value_ptr(view));
        shader0.SetUniform("projection", glm::value_ptr(projection));

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        glBindVertexArray(GL_NONE);
        glUseProgram(GL_NONE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}