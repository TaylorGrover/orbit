#version 460 core
#define NUM_ENTITIES __NUM_ENTITIES__
out vec4 FragColor;


in vec3 ourPos;
in vec3 ourColor;
in vec3 ourNorm;
in flat int instanceID;

uniform sampler2D ourTexture;
uniform bool isLightSource[NUM_ENTITIES]; 

void main()
{
    // ambient light strength
    float ambientStrength = .1;
    // Combine the texture with the positional color scheme
    if(!isLightSource[instanceID]) {
        FragColor = vec4(ourColor * ambientStrength, 1.0f);
    } else {
        FragColor = vec4(ourColor, 1.0f);
    }
}
