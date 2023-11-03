#version 460 core
out vec4 FragColor;

in vec3 ourColor;

uniform float roff = 0.0f;
uniform float goff = 0.0f;
uniform float boff = 0.0f;

void main()
{
    // Adjustable color scheme
    FragColor = vec4(roff, goff, boff, 1.0f);
}
