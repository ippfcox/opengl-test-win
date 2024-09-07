#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "cube.hpp"
#include "spdlog/spdlog.h"

constexpr int SCREEN_WIDTH = 160;
constexpr int SCREEN_HEIGHT = 90;

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
    //                                 model                                  //
    ////////////////////////////////////////////////////////////////////////////

    Model model_nanosuit("../../asset/nanosuit/nanosuit.obj");
    // model_nanosuit.Print();

    ////////////////////////////////////////////////////////////////////////////
    //                            shader                                      //
    ////////////////////////////////////////////////////////////////////////////

    Shader shader;
    int ret = shader.InitByFilename("../../loadmodel/vertex.glsl", "../../loadmodel/fragment.glsl");
    if (ret != 0)
    {
        SPDLOG_ERROR("InitByFilename failed");
        return -1;
    }

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    Cube cube;

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
    //////////////////

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
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera_direction |= CAMERA_MOVE_UP;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera_direction |= CAMERA_MOVE_DOWN;
        if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        {
            camera.ShowInfo();
        }

        camera.ProcessKeyboard(camera_direction, time_delta);

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        shader.Use();
        shader.SetUniform("projection", projection);
        shader.SetUniform("view", view);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        shader.SetUniform("model", model);

        model_nanosuit.Draw(shader);

        glm::mat4 model_cube = glm::mat4(1.0f);

        // cube.Draw(projection, view, model_cube);

        glBindVertexArray(GL_NONE);
        glUseProgram(GL_NONE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}