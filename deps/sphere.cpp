/**
 * Here we create spherical objects that have masses proportional to their 
 * volumes
 */
#include <sphere.hpp>

Sphere::Sphere(float radius, glm::vec3& color, Shader& shader) : Entity(shader)
{
    if(radius <= 0) {
        std::cerr << "Must specify positive radius, default to 1" << std::endl;
        Sphere::radius = 1;
    }
    else {
        Sphere::radius = radius;
    }
    float vol = 4.0 / 4.0 * M_PI * pow(Sphere::radius, 3);
    mass = vol * DENSITY; 
    Sphere::color = color;

    // TODO: Decide if this is fine for most radii
    rings = 100, sectors = 100;
    createVertices();
    createIndices();
}

void Sphere::createVertices() 
{
    if(rings <= 1 || sectors <= 1) {
        std::cerr << "Rings and sectors should be GEQ 2, defaulting to 100\n";
        rings = 100, sectors = 100;
    }
    const float R = 1.0f / (float) (rings - 1);
    const float S = 1.0f / (float) (sectors - 1);
    float x, y, z;
    for(GLuint r = 0; r < rings; r++) {
        for(GLuint s = 0; s < sectors; s++) {
            y = sin(-M_PI / 2 + M_PI * r * R);
            x = cos(2 * M_PI * s * S) * sin(M_PI * R * r);
            z = sin(2 * M_PI * s * S) * sin(M_PI * R * r);
            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
            vertices.push_back(color[0]);
            vertices.push_back(color[1]);
            vertices.push_back(color[2]);
        }
    }
}

void Sphere::createIndices()
{
    GLuint i, j;
    for(i = 0; i < vertices.size() - sectors - 4; i += 3) {
        for(j = 1; j <= 3; j++) {
            indices.push_back(i + j - 1);
            indices.push_back(i + j);
            indices.push_back(i + sectors + j - 1);

            indices.push_back(i + j);
            indices.push_back(i + sectors + j - 1);
            indices.push_back(i + sectors + j);
        }
    }
}