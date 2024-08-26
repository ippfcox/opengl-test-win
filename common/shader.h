#ifndef SHADER_H__
#define SHADER_H__

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#include "glad/glad.h"
#include "GLFW/glfw3.h"

    typedef void *shader;

    shader shader_init(const char *vertex_shader_src, const char *fragment_shader_src);
    shader shader_init_by_filename(const char *vertex_shader_filename, const char *fragment_shader_filename);
    void shader_deinit(shader s);
    void shader_use_program(shader s);
    void shader_set_uniform_int(shader s, int v);
    void shader_set_uniform_float(shader s, const char *name, float f0);
    void shader_set_uniform_4_float(shader s, const char *name, float v0, float v1, float v2, float v3);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SHADER_H__