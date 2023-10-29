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