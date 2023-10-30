#include <entity.hpp>
#include <shader.h>

const glm::mat4 Ident(1.0);

Entity::Entity()
{

}

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
    const GLuint stride = 6;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) (3 * sizeof(float)));
}

std::vector<GLuint>& Entity::getIndices()
{
    return indices;
}

std::vector<float>& Entity::getVertices()
{
    return vertices;
}

void Entity::deleteBuffers()
{
    glDeleteBuffers(1, &VBO);
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
