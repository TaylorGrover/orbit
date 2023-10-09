#version 460 core

layout (location = 0) in vec3 aPos;      // position has attribute position 0
layout (location = 1) in vec3 aColor;    // color in attribute position 1
layout (location = 2) in vec2 aTexCoord; // texture coordinates in position 2

out vec3 ourColor;
out vec3 ourPos;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = aColor;
    ourPos = aPos;
    TexCoord = aTexCoord;
}
