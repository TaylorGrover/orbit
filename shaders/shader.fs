#version 460 core
#define NUM_ENTITIES __NUM_ENTITIES__
#define NUM_LIGHT_SOURCES __NUM_LIGHT_SOURCES__
out vec4 FragColor;

in vec3 ourPos;
in vec3 ourNorm;
in flat int instanceID;

uniform sampler2D ourTexture;
uniform bool isLightSource[NUM_ENTITIES]; 
#if NUM_LIGHT_SOURCES > 0
uniform int lightSourceIndices[NUM_LIGHT_SOURCES];
#else
uniform int lightSourceIndices[1];
#endif
uniform vec3 modelColors[NUM_ENTITIES];
uniform mat4 model[NUM_ENTITIES];

void main()
{
    // ambient light strength
    float ambientStrength = .1;
    vec3 diffuse = vec3(0.0);
    vec3 ourColor = modelColors[instanceID];

    // Combine the texture with the positional color scheme

    if(!isLightSource[instanceID]) {
        // Get location of light sources
        for(int i = 0; i < NUM_LIGHT_SOURCES; i++) {
            vec3 loc = vec3(model[lightSourceIndices[i]] * vec4(0.0, 0.0, 0.0, 1.0));
            vec3 norm = normalize(ourNorm);
            vec3 diff = ourPos - loc;
            float len = length(diff);
            float preDiffuse = max(dot(norm, normalize(diff)), 0.0);
            diffuse += (2050 / len) * preDiffuse * modelColors[lightSourceIndices[i]];
        }
        FragColor = vec4(ourColor * (ambientStrength + diffuse), 1.0f);
    } else {
        FragColor = vec4(ourColor * 2, 1.0f);
    }
}
