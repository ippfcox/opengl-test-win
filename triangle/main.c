#include <stdlib.h>
#include <math.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "shader.h"
#include "log.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

int main()
{
    set_log_level(LOG_LEVEL_DEBUG);

    ////////////////////////////////////////////////////////////////////////////
    //                              glfw                                      //
    ////////////////////////////////////////////////////////////////////////////

    // glfw init
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create window
    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (!window)
    {
        logerror("glfwCreateWindow failed");
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
        logerror("gladLoadGLLoader failed");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////
    //                       VBO/VAO/EBO                                      //
    ////////////////////////////////////////////////////////////////////////////

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); // bind VAO, below vertex attrib will be stored to VAO

    float vertices[] = {
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f};
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)0); // 3 means 3 number in a row
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3};
    GLuint EBO; // store use order indices for vertices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ////////////////////////////////////////////////////////////////////////////
    //                            shader                                      //
    ////////////////////////////////////////////////////////////////////////////

    // shader shader0 = shader_init(vertex_shader_src, fragment_shader_src);
    shader shader0 = shader_init_by_filename("../../triangle/vertex.glsl", "../../triangle/fragment.glsl");
    if (!shader0)
    {
        logerror("init_shader failed");
        return -1;
    }

    shader_use_program(shader0);

    // uniform

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    ////////////////////////////////////////////////////////////////////////////
    //                            glfw loop                                   //
    ////////////////////////////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader_use_program(shader0);
        glBindVertexArray(VAO); // bind VAO, vertex config is ready, EBO is binded automatically
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        glBindVertexArray(GL_NONE);
        glUseProgram(GL_NONE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}