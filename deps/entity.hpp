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
    GLuint VAO, VBO, EBO;
    std::vector<GLuint> indices;
    
    /// @brief Vertices, colors, normals
    std::vector<float> vertices;

    Shader shader;

    // Spatial orientation and position
    glm::mat4 model;

    // Is the model a light source?
    bool isLight;

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
    void scaleModel(const float scale);
    void setProjection(const glm::mat4& projection);
    void setView(const glm::mat4& view);
    void updateLocalAndModel();
    void rotateByDegrees(float degrees, glm::vec3& axis);
    std::vector<GLuint>& getIndices();
    glm::mat4 getModelMatrix();

    GLuint getIndicesSize();
    GLuint getEBO();
    GLuint getVAO();
    void bindEBO();
    void bindVBO();
    void bindVertexArray();
    void drawElements();
    void generateBuffers();
    void deleteBuffers();
    void initObjects();
    void setLight(bool isLight);
    bool getLight();
};


class Sphere: public Entity
{
private:
    float radius;
    float mass;
    GLuint rings, sectors;
    void createVertices() override;
    void createIndices() override;
    glm::vec3 color;

public:
    const float DENSITY = .8f;
    Sphere(float radius, const glm::vec3& color, Shader& shader);

};

#endif