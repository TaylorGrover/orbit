#version 460 core
#define NUM_ENTITIES __NUM_ENTITIES__
#define NUM_LIGHT_SOURCES __NUM_LIGHT_SOURCES__
out vec4 FragColor;

in vec3 ourPos;
in vec3 ourNorm;
in flat int instanceID;

uniform sampler2D ourTexture;
uniform bool isLightSource[NUM_ENTITIES]; 
uniform int lightSourceIndices[NUM_LIGHT_SOURCES];
uniform vec3 modelColors[NUM_ENTITIES];
uniform mat4 model[NUM_ENTITIES]; // Is this the same model uniform in vs?

void main()
{
    // ambient light strength
    float ambientStrength = .3;
    vec3 ambient = ambientStrength * vec3(1.0);

    // attenuation coefficient
    float k = 0.0000; 
    vec3 diffuse = vec3(0.0);
    vec3 ourColor = modelColors[instanceID];

    // Combine the texture with the positional color scheme

    if(!isLightSource[instanceID]) {
        // Compute diffuse lighting from light sources
        vec3 difference = vec3(1.0);
        for(int i = 0; i < NUM_LIGHT_SOURCES; i++) {
            vec3 loc = vec3(model[i] * vec4(0.0, 0.0, 0.0, 1.0));
            difference = loc - ourPos;
            float len = length(difference);
            float preDiffuse = max(dot(normalize(difference), normalize(ourNorm)), 0.0);
            diffuse += 1 / (1 + k * len) * preDiffuse * vec3(1.0);
        }
        FragColor = vec4(ourColor * (ambient + diffuse), 1.0f);
    } else {
        FragColor = vec4(ourColor, 1.0f);
    }
}
