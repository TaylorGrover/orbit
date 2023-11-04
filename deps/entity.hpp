#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <shader.h>
#include <type_traits>
#include <vector>

/**
 * Entity is the base model for an object to be rendered on the GL window. 
 * It contains the vertices and indices to be bound to some VBO and EBO, 
 *  respectively.
*/
class Entity
{
protected: 
    std::vector<GLuint> indices;
    
    /// @brief Vertices, colors, normals
    std::vector<float> vertices;

public:
    Entity();
    
    // Destructor
    virtual ~Entity() {}

    // Should be overridden
    virtual void createVertices() = 0;
    virtual void createIndices() = 0;


    std::vector<GLuint>& getIndices();
    std::vector<float>& getVertices();
};


class Sphere: public Entity
{
private:
    float radius;
    GLuint rings, sectors;
    void createVertices() override;
    void createIndices() override;
    glm::vec3 color;

public:
    Sphere(float radius, const glm::vec3& color);
    Sphere();
};
#endif
