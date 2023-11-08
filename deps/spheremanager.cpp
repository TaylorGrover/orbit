#include <spheremanager.hpp>

#define LARGE_SPHERE

template <typename MatType, GLuint N>
void printMatrix(MatType& matrix)
{
    for(GLuint i = 0; i < N; i++) {
        for(GLuint j = 0; j < N; j++) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

template <typename VecType, GLuint N>
void printVec(VecType& vec)
{
    for(GLuint i = 0; i < N; i++) {
        std::cout << vec[i] << " ";
    }
    std::cout << std::endl;
}

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
    enableAttributes();
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

void SphereManager::enableAttributes()
{
    const GLuint stride = 6;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void *) (3 * sizeof(float)));
}


void SphereManager::initializeSpheres(std::default_random_engine& randEngine, ParameterManager& paramManager)
{
    // Constant; num spheres, G, density
    GLuint N = paramManager.getSphereCount();
    G = paramManager.getGravitationalConstant();
    density = paramManager.getDensity();

    QColor ambientColorQ = paramManager.getAmbientPalette();
    int r, g, b, alpha;
    ambientColorQ.getRgb(&r, &g, &b, &alpha);
    float rf = (float) r / 255.0;
    float gf = (float) g / 255.0;
    float bf = (float) b / 255.0;
    std::cout << rf << " " << gf << " " << bf << std::endl;
    ambientColor = glm::vec3(rf, gf, bf);

    // Initialize the distributions to generate the models
    std::uniform_real_distribution<float> radii_dist(paramManager.getRadiiLowerBound(), paramManager.getRadiiUpperBound());
    // Generate angles theta and phi for location and velocity unit vectors
    std::uniform_real_distribution<float> theta_dist(0, 2 * M_PI);
    std::uniform_real_distribution<float> phi_dist(0, M_PI);
    std::normal_distribution<float> locations_dist(0, paramManager.getLocationSD());
    std::normal_distribution<float> velocities_dist(0, paramManager.getVelocitySD());
    std::uniform_real_distribution<float> light_dist(0, 1);
    std::vector<std::uniform_real_distribution<float>> color_dist = getColorDistribution(
        0.2, 1.0,
        0.6, 0.9,
        0.8, 1.0
    );
    GLuint i, j;
    float theta, phi;
    for(i = 0; i < N; i++) {
        isLightSource.push_back(light_dist(randEngine) <= paramManager.getLightFraction());
        theta = theta_dist(randEngine);
        phi = phi_dist(randEngine);
#ifdef LARGE_SPHERE
        locations.push_back(paramManager.getLocationSD() * glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)));
#else
        locations.push_back(glm::vec3(0.0));
#endif
        theta = theta_dist(randEngine);
        phi = phi_dist(randEngine);
        velocities.push_back(velocities_dist(randEngine) * glm::vec3(cos(theta) * sin(phi), sin(theta) * sin(phi), cos(phi)));
        accelerations.push_back(glm::vec3(0.0));
        colors.push_back(glm::vec3(0.0));
        for(j = 0; j < 3; j++) {
#ifndef LARGE_SPHERE
            locations[i][j] = locations_dist(randEngine);
#endif
            if(isLightSource[i]) {
                colors[i][j] = 1;
            }
            else {
                colors[i][j] = color_dist[j](randEngine);
            }
        }
        radii.push_back(isLightSource[i] ? paramManager.getSunScale() * radii_dist(randEngine) : radii_dist(randEngine));
        models.push_back(glm::translate(glm::mat4(1.0), locations[i]));
        models[i] = glm::scale(models[i], glm::vec3(radii[i]));
        normals.push_back(glm::mat3(glm::transpose(glm::inverse(models[i]))));
        masses.push_back(4.0 * pow(radii[i], 3) * M_PI / 3.0 * density);
        if(isLightSource[i]) {
            lightSourceIndices.push_back(i);
        }
    }
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
    shader.setMat4Array("models", models.data(), models.size());
    shader.setMat3Array("normals", normals.data(), normals.size());
    shader.setIntArray("isLightSource", isLightSource.data(), isLightSource.size());
    shader.setIntArray("lightSourceIndices", lightSourceIndices.data(), lightSourceIndices.size());
    shader.setFloatArray("radii", radii.data(), radii.size());
    shader.setInt("remainingLights", (int) lightSourceIndices.size());
    shader.setVec3("ambientColor", ambientColor);
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
                float newRadius = pow(3.0f * newMass / (4 * M_PI * density), 1.0 / 3.0);
                //glm::vec3 newColor = colorScaler * (1 / masses[i] * colors[j] + 1 / masses[j] * colors[i]);
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
                //colors[keepIndex] = newColor;

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
                //std::cout << accelerations[i].x << std::endl;
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

std::vector<GLuint>& SphereManager::getIndices()
{
    return sphere.getIndices();
}

GLuint SphereManager::getSphereCount()
{
    return models.size();
}
