#ifndef ENTITY_MANAGER
#define ENTITY_MANAGER
#include <type_traits>

/**
 * To enable instancing, this contains all the relevant buffers, the 
 * model/normal matrices, and the entity shader. This also contains 
 * calls to bind the vertex array object.
 * TODO: VAO, VBO, and EBO belong to EntityManager, not Entity
*/
template <typename T>
class EntityManager
{
    static_assert(std::is_base_of<Entity, T>::value, "T must be a subclass of Entity");
    GLuint VAO, VBO, EBO;
    T entity;
    
    std::vector<glm::mat4> models;
    std::vector<glm::mat3> normals;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> locations;
    std::vector<GLint> lightSourceIndices;
    std::vector<GLint> isLightSource;

    void deleteBuffers();

public:
    EntityManager();
    ~EntityManager();
    void setupBuffersAndArrays();
    void generateBuffers();
    void bindVertexArray();
    void bindEBO();
    void bindVBO();

    std::vector<glm::mat4>* getModels();
    std::vector<glm::mat3>* getNormals();
    std::vector<glm::vec3>* getColors();
    
};


#include <entity_manager.tpp>
#endif