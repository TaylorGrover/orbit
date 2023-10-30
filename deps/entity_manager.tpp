#include <random>
// Fraction of entities that are lights
const float LIGHT_FRACTION = .1;

std::vector<std::uniform_real_distribution<float>> getColorDistribution(float rlow, float rhigh, float glow, float ghigh, float blow, float bhigh)
{
    std::vector<std::uniform_real_distribution<float>> color_dist;
    color_dist.push_back(std::uniform_real_distribution<float>(rlow, rhigh));
    color_dist.push_back(std::uniform_real_distribution<float>(glow, ghigh));
    color_dist.push_back(std::uniform_real_distribution<float>(blow, bhigh));
    return color_dist;
}

/**
 * #############
 * EntityManager
 * #############
*/
template <typename T>
EntityManager<T>::EntityManager()
{
    T entity();
    generateBuffers();
    bindVertexArray();
    bindEBO();
    bindVBO();
    // unbind VAO
    glBindVertexArray(0);
}

// Destructor
template <typename T>
EntityManager<T>::~EntityManager() {}

template <typename T>
void EntityManager<T>::bindVertexArray()
{
    glBindVertexArray(VAO);
}
template <typename T>
void EntityManager<T>::bindEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    std::vector<GLuint> indices = entity.getIndices();

    // Do we need to use static draw?
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}

template <typename T>
void EntityManager<T>::bindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::vector<float> vertices = entity.getVertices();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

template <typename T> 
void EntityManager<T>::setupBuffersAndArrays()
{
    bindVertexArray();
}

template <typename T>
void EntityManager<T>::deleteBuffers()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

template <typename T>
void EntityManager<T>::generateBuffers()
{
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
}

template <typename T>
void EntityManager<T>::initializeEntities(GLuint N)
{
    std::uniform_real_distribution<float> radii_dist(1, 5);
    std::normal_distribution<float> locations_dist(0, 50);
    std::uniform_real_distribution<float> light_dist(0, 1);
    std::vector<std::uniform_real_distribution<float>> color_dist = getColorDistribution(
        .1, 1.0,
        .7, .9,
        .8, .9
    );
    GLuint i, j;
    for(i = 0; i < N; i++) {
        //isLightSource.push_back(light_dist())
    }
}