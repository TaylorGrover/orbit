#include <sphere_manager.hpp>

const float DENSITY = .8f;
const float G = 6.64728;

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

SphereManager::SphereManager(const char* vertexPath, const char* fragmentPath)
{
    shader.setShaderPaths(vertexPath, fragmentPath);
    generateBuffers();
    bindVertexArray();
    bindEBO();
    bindVBO();
    // unbind VAO
    glBindVertexArray(0);
}

// Destructor

SphereManager::~SphereManager() {}


void SphereManager::bindVertexArray()
{
    glBindVertexArray(VAO);
}

void SphereManager::bindEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    std::vector<GLuint> indices = sphere.getIndices();

    // Do we need to use static draw?
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}


void SphereManager::bindVBO()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    std::vector<float> vertices = sphere.getVertices();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}


void SphereManager::setupBuffersAndArrays()
{
    bindVertexArray();
}


void SphereManager::deleteBuffers()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}


void SphereManager::generateBuffers()
{
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
}


void SphereManager::initializeSpheres(GLuint N, std::default_random_engine& randEngine)
{
    // Initialize the distributions to generate the models
    std::uniform_real_distribution<float> radii_dist(1, 5);
    std::normal_distribution<float> locations_dist(0, 100);
    std::normal_distribution<float> velocities_dist(0, 5);
    std::uniform_real_distribution<float> light_dist(0, 1);
    std::vector<std::uniform_real_distribution<float>> color_dist = getColorDistribution(
        .1, 1.0,
        .7, .9,
        .8, .9
    );
    GLuint i, j;
    for(i = 0; i < N; i++) {
        isLightSource.push_back(light_dist(randEngine) <= LIGHT_FRACTION);
        locations.push_back(glm::vec3(0.0));
        velocities.push_back(glm::vec3(0.0));
        colors.push_back(glm::vec3(0.0));
        for(j = 0; j < 3; j++) {
            locations[i][j] = locations_dist(randEngine);
            velocities[i][j] = velocities_dist(randEngine);
            if(isLightSource[i]) {
                colors[i][j] = 1;
            }
            else {
                colors[i][j] = color_dist[j](randEngine);
            }
        }
        radii.push_back(isLightSource[i] ? 4 * radii_dist(randEngine) : radii_dist(randEngine));
        models.push_back(glm::scale(glm::mat4(1.0), glm::vec3(radii[i])));
        models[i] = glm::translate(models[i], locations[i]);
        normals.push_back(glm::mat3(glm::transpose(glm::inverse(models[i]))));
        masses.push_back(4.0 * pow(radii[i], 3) * M_PI / 3.0 * DENSITY);
        if(isLightSource[i]) {
            lightSourceIndices.push_back(i);
        }
    }
    //std::cout << lightSourceIndices.size() << std::endl;
    // Shader initialization can only occur once the number of spheres is known
    initializeShader(N);
}

/**
 * @brief SphereManager::setShaderUniforms
 * This just needs the view and the projection matrices, and it will set all
 *   the rest of the instanced model matrices.
 */
void SphereManager::setShaderUniforms(glm::mat4& view, glm::mat4& projection)
{
    shader.setTransform("projection", projection);
    shader.setTransform("view", view);
    shader.setVec3Array("modelColors", colors.data(), colors.size());
    shader.setVec3Array("locations", locations.data(), locations.size());
    shader.setMat4Array("model", models.data(), models.size());
    shader.setMat3Array("normals", normals.data(), normals.size());
    shader.setIntArray("isLightSource", isLightSource.data(), isLightSource.size());
    shader.setIntArray("lightSourceIndices", lightSourceIndices.data(), lightSourceIndices.size());
    shader.setInt("remainingLights", (int) lightSourceIndices.size());
}

void SphereManager::initializeShader(GLuint N)
{
    std::map<std::string, std::string> shaderMacroMap {
        std::make_pair("__NUM_ENTITIES__", std::to_string(N)),
        std::make_pair("__NUM_LIGHT_SOURCES__", std::to_string(lightSourceIndices.size()))
    };
    shader.setPlaceholders(shaderMacroMap);
    shader.compileAndLink();
}
void SphereManager::useShader()
{
    shader.use();
}

/**
 * @brief SphereManager::gravitateSerialAbsorbCollisions
 */
void SphereManager::gravitateSerialAbsorbCollisions(float duration)
{
    glm::vec3 diff;
    float len;
    std::fill(accelerations.begin(), accelerations.end(), glm::vec3(0.0));
    for(GLuint i = 0; i < locations.size() - 1; i++) {
        for(GLuint j = i + 1; j < locations.size(); j++) {
            diff = locations[j] - locations[i];
            len = glm::length(diff);
            if(len <= radii[i] + radii[j]) {
                // One sphere absorbs another. If it's a lightsource,
                //   it will absorb the other object by default.
                float newMass = masses[i] + masses[j];
                float newRadius = pow(3.0f * newMass / (4 * M_PI * DENSITY), 1.0 / 3.0);
                // Conserve momentum
                glm::vec3 newVel = (velocities[i] * masses[i] + velocities[j] * masses[j]) / newMass;
                int eraseIndex, keepIndex;
                if(isLightSource[i]) {
                    // Delete j, because light source absorbs all
                    keepIndex = i;
                    eraseIndex = j;
                }
                else {
                    keepIndex = j;
                    eraseIndex = i;
                }
                masses[keepIndex] = newMass;
                radii[keepIndex] = newRadius;
                velocities[keepIndex] = newVel;

                models.erase(models.begin() + eraseIndex);
                radii.erase(radii.begin() + eraseIndex);
                normals.erase(normals.begin() + eraseIndex);
                locations.erase(locations.begin() + eraseIndex);
                velocities.erase(velocities.begin() + eraseIndex);
                accelerations.erase(accelerations.begin() + eraseIndex);
                masses.erase(masses.begin() + eraseIndex);
                isLightSource.erase(isLightSource.begin() + eraseIndex);
                colors.erase(colors.begin() + eraseIndex);
                // Remove light source indices or decrement them as necessary
                int lightSourceDeleteIndex = -1;
                for(GLuint k = 0; k < lightSourceIndices.size(); k++) {
                    if(lightSourceIndices[k] == eraseIndex) {
                        //lightSourceIndices.erase(lightSourceIndices.begin() + k);
                        lightSourceDeleteIndex = k;
                    }
                    else if(lightSourceIndices[k] > eraseIndex) {
                        lightSourceIndices[k]--;
                    }
                }
                // Delete the light source index if it's being erased
                if(lightSourceDeleteIndex != -1) {
                    lightSourceIndices.erase(lightSourceIndices.begin() + lightSourceDeleteIndex);
                    lightSourceDeleteIndex = -1;
                }
            }
            else {
                glm::vec3 norm = diff / len;
                float k = G / (len * len); // G / r^2
                accelerations[i] += masses[j] * k * norm;
                accelerations[j] += -masses[i] * k * norm;
            }
        }
    }
    for(GLuint i = 0; i < locations.size(); i++) {
        velocities[i] += duration * accelerations[i];
        locations[i] += duration * velocities[i];
        models[i] = glm::translate(glm::mat4(1.0), locations[i]);
        models[i] = glm::scale(models[i], glm::vec3(radii[i]));
    }
}
