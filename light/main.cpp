#include "shader.hpp"
#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "spdlog/spdlog.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

float last_xpos = SCREEN_WIDTH / 2.0f;
float last_ypos = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

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

    // vertex:             texture:
    //       1.0                1.0
    //        |                  |
    // -1.0 --+-- 1.0      0.0 --+-- 1.0
    //        |                  |
    //      -1.0                0.0

    // vertex.x, vertex.y, vertex.z, texture.x, texture.y
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint VAO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glBindVertexArray(VAO_cube);                                                         // bind VAO, below vertex attrib will be stored to VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)0); // 3 means 3 number in a row
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    GLuint VAO_light; // use the same attrib
    glGenVertexArrays(1, &VAO_light);
    glBindVertexArray(VAO_light);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void *)0); // 3 means 3 number in a row
    glEnableVertexAttribArray(0);

    ////////////////////////////////////////////////////////////////////////////
    //                            shader                                      //
    ////////////////////////////////////////////////////////////////////////////

    Shader shader_cube;
    int ret = shader_cube.InitByFilename("../../light/cube_vertex.glsl", "../../light/cube_fragment.glsl");
    if (ret != 0)
    {
        SPDLOG_ERROR("InitByFilename failed");
        return -1;
    }

    Shader shader_light;
    ret = shader_light.InitByFilename("../../light/light_vertex.glsl", "../../light/light_fragment.glsl");
    if (ret != 0)
    {
        SPDLOG_ERROR("InitByFilename failed");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); });

    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        float xoff = xpos - last_xpos;
        float yoff = last_ypos - ypos;
        last_xpos = xpos;
        last_ypos = ypos;

        camera.ProcessMouseMovement(xoff, yoff, false);
    });

    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoff, double yoff) {
        camera.ProcessMouseScroll(yoff);
    });

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    ////////////////////////////////////////////////////////////////////////////
    //                           matrix                                       //
    ////////////////////////////////////////////////////////////////////////////

    float radius = 2.0f;
    float theta = 0;
    float theta_step = 0.1;

    ////////////////////////////////////////////////////////////////////////////
    //                            glfw loop                                   //
    ////////////////////////////////////////////////////////////////////////////

    float time_delta = 0.0f, time_last = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float time_curr = (float)glfwGetTime();
        time_delta = time_curr - time_last;
        time_last = time_curr;

        unsigned camera_direction = 0;
        // process input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera_direction |= CAMERA_MOVE_FORWARD;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera_direction |= CAMERA_MOVE_BACKWARD;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera_direction |= CAMERA_MOVE_LEFT;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera_direction |= CAMERA_MOVE_RIGHT;
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        {
            camera.ShowInfo();
        }

        camera.ProcessKeyboard(camera_direction, time_delta);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 position_light(radius * cos(theta), radius * sin(theta), 2.0f);
        theta += time_delta * theta_step;

        shader_cube.Use();
        shader_cube.SetUniform("objectColor", 1.0f, 0.5f, 0.31f);
        shader_cube.SetUniform("lightColor", 1.0f, 1.0f, 1.0f);
        shader_cube.SetUniform("lightPos", position_light);
        shader_cube.SetUniform("viewPos", camera.GetPosition());

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
        shader_cube.SetUniform("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        shader_cube.SetUniform("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        shader_cube.SetUniform("model", model);

        glBindVertexArray(VAO_cube); // bind VAO, vertex config is ready, EBO is binded automatically
        glDrawArrays(GL_TRIANGLES, 0, 36);

        shader_light.Use();
        shader_light.SetUniform("projection", projection);
        shader_light.SetUniform("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, position_light);
        model = glm::scale(model, glm::vec3(0.2f));
        shader_light.SetUniform("model", model);

        glBindVertexArray(VAO_light); // bind VAO, vertex config is ready, EBO is binded automatically
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glBindVertexArray(GL_NONE);
        glUseProgram(GL_NONE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}