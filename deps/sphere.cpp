/**
 * Here we create spherical objects that have masses proportional to their 
 * volumes
 */
#include <entity.hpp>

Sphere::Sphere()
{
    radius = 1;
    color = glm::vec3(1.0);
    float vol = 4.0 / 3.0 * M_PI;
    mass = vol * DENSITY; 
    rings = 80, sectors = 80;
    createVertices();
    createIndices();
}

Sphere::Sphere(float radius, const glm::vec3& color)
{
    if(radius <= 0) {
        std::cerr << "Must specify positive radius, default to 1" << std::endl;
        Sphere::radius = 1;
    }
    else {
        Sphere::radius = radius;
    }
    float vol = 4.0 / 3.0 * M_PI * pow(Sphere::radius, 3);
    mass = vol * DENSITY; 
    Sphere::color = color;

    // TODO: Decide if this is fine for most radii
    rings = 80, sectors = 80;
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
            vertices.push_back(.4);
            vertices.push_back(.8);
            vertices.push_back(.9);
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            //std::cout << x << " " << y << " " << z << std::endl;
        }
    }
}

void Sphere::createIndices()
{
    for(GLuint r = 0; r < rings - 1; r++) {
        for(GLuint s = 0; s < sectors - 1; s++) {
            indices.push_back(r * sectors + s);       // Current point
            indices.push_back(r * sectors + s + 1);   // Next point in the same row
            indices.push_back((r + 1) * sectors + s); // Corresponding point in the next row

            indices.push_back(r * sectors + s + 1);
            indices.push_back((r + 1) * sectors + s + 1);
            indices.push_back((r + 1) * sectors + s);
        }
    }
}