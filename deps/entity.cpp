#include <entity.hpp>
#include <shader.h>

const glm::mat4 Ident(1.0);

Entity::Entity()
{

}

std::vector<GLuint>& Entity::getIndices()
{
    return indices;
}

std::vector<float>& Entity::getVertices()
{
    return vertices;
}
