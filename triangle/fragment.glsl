#version 330 core

layout (location = 1) in vec3 texColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(texColor, 1.0);
}
