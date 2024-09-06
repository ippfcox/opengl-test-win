#include "shader.hpp"
#include "camera.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "spdlog/spdlog.h"

constexpr int SCREEN_WIDTH = 1280;
constexpr int SCREEN_HEIGHT = 720;

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
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint VAO_cube;
    glGenVertexArrays(1, &VAO_cube);
    glBindVertexArray(VAO_cube);                                                         // bind VAO, below vertex attrib will be stored to VAO
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)0); // 3 means 3 number in a row
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint VAO_light; // use the same attrib
    glGenVertexArrays(1, &VAO_light);
    glBindVertexArray(VAO_light);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void *)0); // 3 means 3 number in a row
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
    //                                 texture                                //
    ////////////////////////////////////////////////////////////////////////////

    int texture_width, texture_height, texture_channels;
    stbi_uc *data = stbi_load("../../asset/container2.png", &texture_width, &texture_height, &texture_channels, 0);
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

    shader_cube.Use();
    shader_cube.SetUniform("material.diffuse", 0); // sampler2D aTexture <-> GL_TEXTURE0
    glUseProgram(GL_NONE);

    data = stbi_load("../../asset/container2_specular.png", &texture_width, &texture_height, &texture_channels, 0);
    if (!data)
    {
        SPDLOG_ERROR("stbi_load failed");
        return -1;
    }
    texture_format = texture_channels == 3 ? GL_RGB : GL_RGBA;

    GLuint texture_1;
    glGenTextures(1, &texture_1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, texture_width, texture_height, 0, texture_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(texture_1);
    stbi_image_free(data);

    shader_cube.Use();
    shader_cube.SetUniform("material.specular", 1); // sampler2D aTexture <-> GL_TEXTURE0
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

    glm::vec3 point_light_positions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f),
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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader_cube.Use();
        shader_cube.SetUniform("material.ambient", 1.0f, 0.5f, 0.31f);
        shader_cube.SetUniform("material.diffuse", 1.0f, 0.5f, 0.31f);
        shader_cube.SetUniform("material.specular", 0.5f, 0.5f, 0.5f);
        shader_cube.SetUniform("material.shininess", 32.0f);
        // directional light
        shader_cube.SetUniform("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader_cube.SetUniform("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        shader_cube.SetUniform("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        shader_cube.SetUniform("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        shader_cube.SetUniform("pointLights[0].position", point_light_positions[0]);
        shader_cube.SetUniform("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        shader_cube.SetUniform("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        shader_cube.SetUniform("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        shader_cube.SetUniform("pointLights[0].constant", 1.0f);
        shader_cube.SetUniform("pointLights[0].linear", 0.09f);
        shader_cube.SetUniform("pointLights[0].quadratic", 0.032f);
        // point light 2
        shader_cube.SetUniform("pointLights[1].position", point_light_positions[1]);
        shader_cube.SetUniform("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        shader_cube.SetUniform("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        shader_cube.SetUniform("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        shader_cube.SetUniform("pointLights[1].constant", 1.0f);
        shader_cube.SetUniform("pointLights[1].linear", 0.09f);
        shader_cube.SetUniform("pointLights[1].quadratic", 0.032f);
        // point light 3
        shader_cube.SetUniform("pointLights[2].position", point_light_positions[2]);
        shader_cube.SetUniform("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        shader_cube.SetUniform("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        shader_cube.SetUniform("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        shader_cube.SetUniform("pointLights[2].constant", 1.0f);
        shader_cube.SetUniform("pointLights[2].linear", 0.09f);
        shader_cube.SetUniform("pointLights[2].quadratic", 0.032f);
        // point light 4
        shader_cube.SetUniform("pointLights[3].position", point_light_positions[3]);
        shader_cube.SetUniform("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        shader_cube.SetUniform("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        shader_cube.SetUniform("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        shader_cube.SetUniform("pointLights[3].constant", 1.0f);
        shader_cube.SetUniform("pointLights[3].linear", 0.09f);
        shader_cube.SetUniform("pointLights[3].quadratic", 0.032f);

        shader_cube.SetUniform("viewPos", camera.GetPosition());

        glBindVertexArray(VAO_cube); // bind VAO, vertex config is ready, EBO is binded automatically

        glm::mat4 projection = glm::perspective(glm::radians(camera.GetFOV()), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
        shader_cube.SetUniform("projection", projection);
        glm::mat4 view = camera.GetViewMatrix();
        shader_cube.SetUniform("view", view);
        glm::mat4 model;
        for (int i = 0; i < sizeof(cube_positions) / sizeof(cube_positions[0]); ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cube_positions[i]);
            float angle = glm::radians(20.0f * i) + (float)glfwGetTime() * glm::radians(18.0f);
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            shader_cube.SetUniform("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        shader_light.Use();
        glBindVertexArray(VAO_light); // bind VAO, vertex config is ready, EBO is binded automatically
        shader_light.SetUniform("projection", projection);
        shader_light.SetUniform("view", view);
        for (int i = 0; i < sizeof(point_light_positions) / sizeof(point_light_positions[0]); ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, point_light_positions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            shader_light.SetUniform("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(GL_NONE);
        glUseProgram(GL_NONE);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}