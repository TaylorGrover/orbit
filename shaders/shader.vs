#version 460 core
#define NUM_ENTITIES __NUM_ENTITIES__

layout (location = 0) in vec3 aPos;      // position has attribute position 0
layout (location = 1) in vec3 aColor;    // color in attribute position 1
layout (location = 2) in vec3 aNormal;   // normal vectors

out vec3 ourPos;
out vec3 ourNorm;
out vec2 TexCoord;
out flat int instanceID;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model[NUM_ENTITIES];

void main()
{
    mat4 instanceModel = model[gl_InstanceID];
    gl_Position = projection * view * instanceModel * vec4(aPos, 1.0);
    ourPos = vec3(instanceModel * vec4(aPos, 1.0));
    ourNorm = aNormal;
    instanceID = gl_InstanceID;
}
