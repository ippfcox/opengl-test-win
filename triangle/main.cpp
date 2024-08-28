#include "shader.hpp"
#include "camera.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "spdlog/spdlog.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

float last_xpos = SCREEN_WIDTH / 2.0f;
float last_ypos = SCREEN_HEIGHT / 2.0f;

Camera camera;

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
    // model
    // glm::mat4 model(1.0f);
    // view
    // glm::mat4 view(1.0f);
    // projection
    // glm::mat4 projection(1.0f);

    glm::vec3 cube_positions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
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

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader0.Use();
        glBindVertexArray(VAO); // bind VAO, vertex config is ready, EBO is binded automatically

        glm::mat4 view(1.0f);
        view = camera.GetViewMatrix();
        shader0.SetUniform("view", glm::value_ptr(view));

        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
        shader0.SetUniform("projection", glm::value_ptr(projection));

        for (int i = 0; i < sizeof(cube_positions) / sizeof(glm::vec3); ++i)
        {
            glm::mat4 model(1.0f);
            float angle_delta = i % 3 == 0 ? (float)glfwGetTime() * glm::radians(9.0f) : 0;
            model = glm::translate(model, cube_positions[i]);
            model = glm::rotate(model, glm::radians(20.0f * i) + angle_delta, glm::vec3(1.0f, 0.3f, 0.5f));
            shader0.SetUniform("model", glm::value_ptr(model));

            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(GL_NONE);
        glUseProgram(GL_NONE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}