#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <entity.hpp>
#include <cmath>

const float DENSITY = .8f;

class Sphere: public Entity
{
private:
    float radius;
    float mass;
    GLuint rings, sectors;
    void createVertices() override;
    void createIndices() override;

public:
    Sphere(float radius, Shader& shader);
};

#endif