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
uniform int lightSourceIndices[1] = {0};
#endif 

uniform vec3 modelColors[NUM_ENTITIES];
uniform mat4 model[NUM_ENTITIES]; // Is this the same model uniform in vs?
uniform vec3 locations[NUM_ENTITIES];

// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Compound versions of the hashing algorithm I whipped together.
uint hash( uvec2 v ) { return hash( v.x ^ hash(v.y)                         ); }
uint hash( uvec3 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z)             ); }
uint hash( uvec4 v ) { return hash( v.x ^ hash(v.y) ^ hash(v.z) ^ hash(v.w) ); }

// Generate random numbers
// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) 
{
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}

float rand(float a, float b, uint m) 
{
    return (b - a) * floatConstruct(m) + a;
}

void main()
{
    // ambient light strength
    float ambientStrength = .2;
    vec3 ambientColor = vec3(0.75, 0.9, 1.0);
    vec3 ambient = ambientStrength * ambientColor;

    // attenuation coefficient
    float k = 0.001; 
    vec3 diffuse = vec3(0.0);
    vec3 ourColor = modelColors[instanceID];

    // Combine the texture with the positional color scheme

    if(!isLightSource[instanceID]) {
        // Compute diffuse lighting from light sources
        vec3 difference = vec3(1.0);
        for(int i = 0; i < NUM_LIGHT_SOURCES; i++) {
            vec3 loc = vec3(model[lightSourceIndices[i]] * vec4(0.0, 0.0, 0.0, 1.0));
            difference = loc - ourPos;
            float len = length(difference);
            float preDiffuse = max(dot(normalize(difference), ourNorm), 0.0);
            diffuse += 1 / (1 + k * len) * preDiffuse * vec3(1.0);
        }
        FragColor = vec4(ourColor * (ambient + diffuse), 1.0f);
    } 
    else {
        FragColor = vec4(ourColor * ambientColor, 1.0);
    }
}
