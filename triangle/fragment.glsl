#version 330 core

uniform sampler2D aTexture0;
uniform sampler2D aTexture1;

in vec3 texColor;
in vec2 texCoord;

out vec4 FragColor;

void main()
{
    // FragColor = vec4(texColor, 1.0);
    FragColor = mix(texture(aTexture0, texCoord), texture(aTexture1, texCoord), 0.2);
}
