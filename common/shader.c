#include <corecrt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "shader.h"
#include "log.h"

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

static GLuint create_shader(GLenum type, const char *shader_src)
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

static void delete_shader(GLuint shader)
{
    glDeleteShader(shader);
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

struct shader_context
{
    GLuint vertex_shader, fragment_shader, program;
};

shader shader_init(const char *vertex_shader_src, const char *fragment_shader_src)
{
    struct shader_context *ctx = calloc(1, sizeof(struct shader_context));
    if (!ctx)
    {
        logerror("calloc failed");
        goto error1;
    }

    ctx->vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_shader_src);
    if (ctx->vertex_shader == 0)
    {
        logerror("create_shader failed");
        goto error2;
    }

    ctx->fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if (ctx->fragment_shader == 0)
    {
        logerror("create_shader failed");
        goto error3;
    }

    ctx->program = link_program(ctx->vertex_shader, ctx->fragment_shader);
    if (ctx->program == 0)
    {
        logerror("link_program failed");
        goto error4;
    }

    return ctx;

error4:
    delete_shader(ctx->fragment_shader);
error3:
    delete_shader(ctx->vertex_shader);
error2:
    free(ctx);
error1:
    return NULL;
}

shader shader_init_by_filename(const char *vertex_shader_filename, const char *fragment_shader_filename)
{
    FILE *vertex_shader_fp = NULL, *fragment_shader_fp = NULL;

#if defined(_WIN32)
    errno_t err = fopen_s(&vertex_shader_fp, vertex_shader_filename, "rb");
    if (err != 0)
    {
        logerror("fopen_s %s failed, err: %d", vertex_shader_filename, err);
        goto error1;
    }
    err = fopen_s(&fragment_shader_fp, fragment_shader_filename, "rb");
    if (err != 0)
    {
        logerror("fopen_s %s failed, err: %d", fragment_shader_filename, err);
        goto error2;
    }
#else
    fprintf(stderr, "alskdfjalskdjflasdjflajsdflajsdlfjasljdfl");
    exit(-1);
#endif

    fseek(vertex_shader_fp, 0, SEEK_END);
    int vertex_shader_length = ftell(vertex_shader_fp);
    rewind(vertex_shader_fp);
    char *vertex_shader_buffer = calloc(1, vertex_shader_length + 1);
    if (!vertex_shader_buffer)
    {
        logerror("calloc failed");
        goto error3;
    }
    fread(vertex_shader_buffer, 1, vertex_shader_length, vertex_shader_fp);

    fseek(fragment_shader_fp, 0, SEEK_END);
    int fragment_shader_length = ftell(vertex_shader_fp);
    rewind(fragment_shader_fp);
    char *fragment_shader_buffer = calloc(1, fragment_shader_length + 1);
    if (!fragment_shader_buffer)
    {
        logerror("calloc failed");
        goto error4;
    }
    fread(fragment_shader_buffer, 1, fragment_shader_length, fragment_shader_fp);

    shader s = shader_init(vertex_shader_buffer, fragment_shader_buffer);
    if (!s)
    {
        logerror("shader_init failed");
        goto error5;
    }

    free(vertex_shader_buffer);
    free(fragment_shader_buffer);

    return s;

error5:
    free(fragment_shader_buffer);
error4:
    free(vertex_shader_buffer);
error3:
    fclose(fragment_shader_fp);
error2:
    fclose(vertex_shader_fp);
error1:
    return NULL;
}

void shader_deinit(shader s)
{
    struct shader_context *ctx = s;

    if (!ctx)
        return;

    if (ctx->program)
        glDeleteProgram(ctx->program);
    if (ctx->fragment_shader)
        glDeleteShader(ctx->fragment_shader);
    if (ctx->vertex_shader)
        glDeleteShader(ctx->vertex_shader);

    free(ctx);
}

void shader_use_program(shader s)
{
    struct shader_context *ctx = s;

    if (!ctx || ctx->program == 0)
    {
        logerror("invalid failed");
        return;
    }

    glUseProgram(ctx->program);
}

void shader_set_uniform_int(shader s, int v);
void shader_set_uniform_float(shader s, float v);