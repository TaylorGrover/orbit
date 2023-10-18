#version 460 core

layout (location = 0) in vec3 aPos;      // position has attribute position 0
layout (location = 1) in vec3 aColor;    // color in attribute position 1
layout (location = 2) in vec2 aTexCoord; // texture coordinates in position 2

out vec3 ourColor;
out vec3 ourPos;
out vec2 TexCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model[25];
uniform vec3 modelColors[25];

void main()
{
    mat4 instanceModel = model[gl_InstanceID];
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
    ourColor = aColor * modelColors[gl_InstanceID];
    ourPos = aPos;
    TexCoord = aTexCoord;
}
