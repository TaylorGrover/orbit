#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <vector>

class Entity
{
protected: 
    GLuint VBO, VAO, EBO;

    std::vector<GLuint> indices;
    
    /// @brief Vertices, colors, normals
    std::vector<float> vertices;

    Shader shader;

    // Spatial orientation and position
    glm::mat4 model;

public:
    Entity(Shader& shader);
    
    // Destructor
    virtual ~Entity() {}

    // Should be overridden
    virtual void createVertices() = 0;
    virtual void createIndices() = 0;

    /// Setting up buffer objects
    void setupBuffersAndArrays();
    void enableAttributes();

    void useShader();
    void setPosition(const glm::vec3& pos);
    void setProjection(const glm::mat4& projection);
    void setView(const glm::mat4& view);
    void updateLocalAndModel();
    void rotateByDegrees(float degrees, glm::vec3& axis);
    std::vector<GLuint>& getIndices();
    glm::mat4 getModelMatrix();

    GLuint getEBO();
    GLuint getVAO();
    void bindEBO();
    void bindVBO();
    void bindVertexArray();
    void drawElements();
    void generateBuffers();
    void initObjects();
};

#endif