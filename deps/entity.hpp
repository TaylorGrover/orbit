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
    GLuint VAO, VBO, EBO;
    std::vector<GLuint> indices;
    
    /// @brief Vertices, colors, normals
    std::vector<float> vertices;

    // Spatial orientation and position
    glm::mat4 model;

    // Is the model a light source?
    bool isLight;

public:
    Entity();
    
    // Destructor
    virtual ~Entity() {}

    // Should be overridden
    virtual void createVertices() = 0;
    virtual void createIndices() = 0;


    /// Setting up buffer objects
    void setupBuffersAndArrays();
    void enableAttributes();

    std::vector<GLuint>& getIndices();
    std::vector<float>& getVertices();
    glm::mat4 getModelMatrix();

    void bindEBO();
    void bindVBO();
    void bindVertexArray();
    void drawElements();
    void generateBuffers();
    void deleteBuffers();
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