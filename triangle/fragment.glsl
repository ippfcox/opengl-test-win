#version 330 core

uniform sampler2D aTexture;

in vec3 texColor;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(texColor, 1.0);
    FragColor = texture(aTexture, texCoord) * vec4(texColor, 1.0);
}
