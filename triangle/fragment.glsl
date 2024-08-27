#version 330 core

uniform sampler2D aTexture0;

in vec2 texCoord;

out vec4 FragColor;

void main()
{
    FragColor = texture(aTexture0, texCoord);
}
