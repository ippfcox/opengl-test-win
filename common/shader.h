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
    void shader_deinit(shader s);
    void shader_use_program(shader s);
    void shader_set_uniform_int(shader s, int v);
    void shader_set_uniform_float(shader s, float v);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // SHADER_H__