#ifndef SPHERE_MANAGER
#define SPHERE_MANAGER
#include <entity.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <shader.h>
#include <type_traits>

// Fraction of entities that are lights
const float LIGHT_FRACTION = .05;
const float DENSITY = .8f;
const float G = 6.64728e-1;


/**
 * To enable instancing, this contains all the relevant buffers, the 
 * model/normal matrices, and the entity shader. This also contains 
 * calls to bind the vertex array object.
 * TODO: VAO, VBO, and EBO belong to SphereManager, not Entity
*/
class SphereManager
{
    GLuint VAO, VBO, EBO;
    Sphere sphere;
    Shader shader;
    
    std::vector<glm::mat4> models;
    std::vector<glm::mat3> normals;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> locations;
    std::vector<glm::vec3> velocities;
    std::vector<glm::vec3> accelerations;
    std::vector<float> masses;
    std::vector<float> radii;
    std::vector<GLint> lightSourceIndices;
    std::vector<GLint> isLightSource;

    void initializeShader(GLuint N);
    void deleteBuffers();

public:
    SphereManager(const char* vertexPath, const char* fragmentPath);
    ~SphereManager();
    void generateBuffers();
    void bindVertexArray();
    void bindEBO();
    void bindVBO();
    void enableAttributes();
    void setShaderUniforms(glm::mat4& view, glm::mat4& projection);
    void useShader();

    void initializeSpheres(GLuint N, std::default_random_engine& randEngine);
    void gravitateSerialAbsorbCollisions(float duration);
    void gravitateParallel();

    std::vector<GLuint>& getIndices();
    GLuint getSphereCount();
    
};

#endif
