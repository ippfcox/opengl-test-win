#include <stdlib.h>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
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

static int check_error(GLuint x)
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
        logerror("invalid x");
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
            char *info_log = calloc(1, info_len);
            glGetInfoLog(x, info_len, NULL, info_log);
            logerror("failed: %s", info_log);
            free(info_log);
        }
        glDelete(x);
        return -1;
    }

    return 0;
}

static GLuint load_shader(GLenum type, const char *shader_src)
{
    GLuint shader = glCreateShader(type);
    if (shader == 0)
    {
        logerror("glCreateShader failed");
        return 0;
    }

    // load the shader source
    glShaderSource(shader, 1, &shader_src, NULL);

    // compile the shader
    glCompileShader(shader);

    if (check_error(shader) != 0)
    {
        logerror("error accured");
        return 0;
    }

    return shader;
}

static GLuint link_program(GLuint vertex_shader, GLuint fragment_shader)
{
    GLuint program = glCreateProgram();
    if (program == 0)
    {
        logerror("glCreateProgram failed");
        return 0;
    }

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glLinkProgram(program);

    if (check_error(program) != 0)
    {
        logerror("error occured");
        return 0;
    }

    return program;
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
        0.5f, 0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f};
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void *)0); // 3 means 3 number in a row
    glEnableVertexAttribArray(0);

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

    char vertex_shader_src[] =
        "#version 330 core                                                   \n"
        "layout (location = 0) in vec3 aPos;                                 \n"
        "void main()                                                         \n"
        "{                                                                   \n"
        "    gl_Position = vec4(aPos, 1.0);                                  \n"
        "}                                                                  \n";
    GLuint vertex_shader = load_shader(GL_VERTEX_SHADER, vertex_shader_src);
    if (vertex_shader == 0)
    {
        logerror("load_shader vertex failed");
        return -1;
    }

    char fragment_shader_src[] =
        "#version 330 core                                                   \n"
        "out vec4 FragColor;                                                 \n"
        "void main()                                                         \n"
        "{                                                                   \n"
        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);                       \n"
        "}                                                                  \n";
    GLuint fragment_shader = load_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (fragment_shader == 0)
    {
        logerror("loader_shader fragment failed");
        return -1;
    }

    GLuint program = link_program(vertex_shader, fragment_shader);
    if (program == 0)
    {
        logerror("link_program failed");
        return -1;
    }

    glUseProgram(program); // use here for glUniform

    // uniform

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ////////////////////////////////////////////////////////////////////////////
    //                            glfw loop                                   //
    ////////////////////////////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
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