#include <entity.hpp>
#include <shader.h>

const glm::mat4 Ident(1.0);

Entity::Entity(Shader& shader) : shader(shader) {}

void Entity::setupBuffersAndArrays()
{
    bindVertexArray();
    bindEBO();
    bindVBO();
}

void Entity::generateBuffers()
{
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &EBO);
}

void Entity::enableAttributes()
{
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
}

void Entity::useShader()
{
    shader.use();
}

void Entity::setPosition(const glm::vec3& pos)
{
    model = glm::translate(Ident, pos);
}

void Entity::setView(const glm::mat4& view)
{
    shader.setTransform("view", view);
}

void Entity::setProjection(const glm::mat4& projection)
{
    shader.setTransform("projection", projection);
}

void Entity::updateLocalAndModel()
{
    shader.setTransform("model", model);
}

void Entity::rotateByDegrees(float degrees, glm::vec3& axis)
{
    model = glm::rotate(model, glm::radians(degrees), axis);
}

std::vector<GLuint>& Entity::getIndices()
{
    return Entity::indices;
}

GLuint Entity::getEBO()
{
    return EBO;
}

GLuint Entity::getVAO()
{
    return VAO;
}

void Entity::bindVertexArray()
{
    glBindVertexArray(VAO);
}

void Entity::bindEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

void Entity::bindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void Entity::drawElements()
{
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

glm::mat4 Entity::getModelMatrix()
{
    return model;
}