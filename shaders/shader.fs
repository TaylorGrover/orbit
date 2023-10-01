#version 460 core
out vec4 FragColor;

in vec3 ourPos;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    // Combine the texture with the positional color scheme
    FragColor = vec4(ourColor, 1.0f);
}
