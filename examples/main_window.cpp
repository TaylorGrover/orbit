#include <iostream>
#include <chrono>
#include <cmath>
#include <camera.hpp>
#include <entity.hpp>
#include <entity_manager.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <input.hpp>
#include <qtwindow.h>
#include <map>
#include <shader.h>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define DEBUG_OFF

const GLuint SCR_WIDTH = 2400;
const GLuint SCR_HEIGHT = 1600;

const float DENSITY = 0.8f;

#ifndef DEBUG_ON
const GLuint NUM_SPHERES = 1 << 6;
#else
const GLuint NUM_SPHERES = 2;
#endif

const char *WINDOW_TITLE = "Orbit";
const char *VERTEX_PATH = "shaders/shader.vs";
const char *FRAG_PATH = "shaders/shader.fs";

// Gravitational constant (scaled by 10^18) N * m^2 / kg^2
const float G = 6.64728e-2;

// FOV
const float FOV = 85;

// Near and far clipping plane distances
const float NEAR = 0.01f;
const float FAR = 1000;

// Starting camera position
#ifdef DEBUG_ON
glm::vec3 camera_position(0.0, -50.0, 0.0);
#else
glm::vec3 camera_position(0.0, 0.0, 0.0);
#endif

// Cursor and keyboard input handling
Input keyCursorInput(SCR_WIDTH, SCR_HEIGHT);

// Camera rotation globals
Camera camera(keyCursorInput, camera_position, glm::mat4(1.0), FAR);

    
// Normal distribution
//const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
const unsigned seed = 9;
std::default_random_engine rand_engine(seed);

void configureWindowHints()
{
    // glfw: configure window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    // Using GLFW_VERSION_MINOR makes the window uncreatable for some reason, so
    //   a hardcoded 3 is used
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // Depth Buffer Allocation
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    keyCursorInput.keyCallback(window, key, scancode, action, mods);
}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    keyCursorInput.cursorPositionCallback(window, xpos, ypos);
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // ensure viewport matches new window dimensions; note that width and 
    //   height will be signficantly larger than specified on retina displays
    // Specify location of the lower left corner of the first (x, y), then
    //   the dimensions
    glViewport(0, 0, width, height);
}

/**
 * Naive brute force gravity calculations and collision detection
 * TODO: Find faster solution later
*/
void updateModels(std::vector<glm::mat4>& models, std::vector<glm::mat3>& normals, std::vector<glm::vec3>& locations, std::vector<glm::vec3>& velocities, std::vector<glm::vec3>& accelerations, std::vector<float> masses, std::vector<float>& radii, std::vector<GLint>& isLightSource, std::vector<GLint>& lightSourceIndices, float duration, float accumulated, int &interval)
{
    int tmp;
    //std::cout << accumulated << std::endl;
        interval++;
        glm::vec3 diff;
        float len;
        std::fill(accelerations.begin(), accelerations.end(), glm::vec3(0.0));
        for(GLuint i = 0; i < locations.size() - 1; i++) {
            for(GLuint j = i + 1; j < locations.size(); j++) {
                diff = locations[j] - locations[i];
                len = glm::length(diff);
                // TODO: Collisions
                if(len <= radii[i] + radii[j]) {
                    glm::vec3 momentum = masses[i] * velocities[i] + masses[j] * velocities[j];
                    glm::vec3 newVel = momentum / (masses[i] + masses[j]);
                    velocities[i] = newVel;
                    velocities[j] = newVel;
                    /*glm::vec3 midpoint = (locations[i] + locations[j]) / 2.0f;
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
                    locations[keepIndex] = midpoint;
                    radii[keepIndex] = newRadius;
                    velocities[keepIndex] = newVel;

                    models.erase(models.begin() + eraseIndex);
                    normals.erase(normals.begin() + eraseIndex);
                    locations.erase(locations.begin() + eraseIndex);
                    velocities.erase(velocities.begin() + eraseIndex);
                    accelerations.erase(accelerations.begin() + eraseIndex);
                    masses.erase(masses.begin() + eraseIndex);
                    radii.erase(radii.begin() + eraseIndex);
                    isLightSource.erase(isLightSource.begin() + eraseIndex);
                    for(GLuint k = 0; k < lightSourceIndices.size(); k++) {
                        if(lightSourceIndices[k] == eraseIndex) {
                            lightSourceIndices.erase(lightSourceIndices.begin() + k);
                        }
                        else if(lightSourceIndices[k] > eraseIndex) {
                            lightSourceIndices[k]--;
                        }
                    }
                    //std::cout << accelerations[j][0] << " " << accelerations[j][1] << " " << accelerations[j][2] << std::endl;
                    */
                }
                else {
                    glm::vec3 norm = diff / len;
                    float k = G / (len * len); // G / r^2
                    accelerations[i] += masses[j] * k * norm;
                    accelerations[j] += -masses[i] * k * norm;
                }
            }
            //std::cout << accelerations[i][0] << " " << accelerations[i][1] << " " << accelerations[i][2] << std::endl;
        }
        for(GLuint i = 0; i < locations.size(); i++) {
            velocities[i] += duration * accelerations[i];
            locations[i] += duration * velocities[i];
            models[i] = glm::translate(glm::mat4(1.0), locations[i]);
            models[i] = glm::scale(models[i], glm::vec3(radii[i]));
        }
}

template <typename T, GLuint n>
void printNormal(T normal)
{
    for(GLuint i = 0; i < n; i++) {
        for(GLuint j = 0; j < n; j++) {
            std::cout << normal[i][j] << " ";
        }
        std::cout << "\n";
    }
}

int main()
{
    // Random seed
    std::srand(1);
    if(!glfwInit()) {
        std::cerr << "Unable to initialize GLFW\n";
        return -1;
    }
    
    // glfwWindowHint calls
    configureWindowHints();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE, NULL, NULL);
    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL);
    if(window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }


    // Register events callback
    glfwSetKeyCallback(window, keyCallback);

    // Capture cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    if(glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }


    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // glad: load all OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // Wireframe mode (must be called after gladLoadGLLoader
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    EntityManager<Sphere> sphereManager;

    std::vector<glm::mat4> models(NUM_SPHERES, glm::mat4(1.0));
    std::vector<float> radii(NUM_SPHERES);
    std::vector<glm::mat3> normals(NUM_SPHERES);
    std::vector<glm::vec3> locations(NUM_SPHERES);
    std::vector<glm::vec3> velocities(NUM_SPHERES);
    std::vector<glm::vec3> accelerations(NUM_SPHERES, glm::vec3(0.0));
    std::vector<float> masses(NUM_SPHERES);
    std::vector<glm::vec3> colors(NUM_SPHERES, glm::vec3(0.0));
    // Essentially a boolean array
    std::vector<GLint> isLightSource(NUM_SPHERES, 0);
    std::vector<GLint> lightSourceIndices;

#ifdef DEBUG_ON
    // For debugging
    float start = 17.0;
    locations[0] = glm::vec3(0.0);
    locations[1] = glm::vec3(-start, 0.0, 0.0);

    radii[0] = 10;
    radii[1] = 1;
    models[0] = glm::scale(glm::mat4(1.0), glm::vec3(5));
    models[0] = glm::translate(models[0], locations[0]);

    masses[0] = pow(radii[0], 3) * M_PI * 4 / 3.0 * .8;

    models[1] = glm::scale(glm::mat4(1.0), glm::vec3(radii[1]));
    models[1] = glm::translate(models[1], locations[1]);

    masses[1] = pow(radii[1], 3) * 4.0 * M_PI / 3 * .8;
    //models[1] = glm::rotate(models[1], glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));

    velocities[0] = glm::vec3(0.0);
    velocities[1] = glm::vec3(0.0, 0.0, -sqrt(masses[0] * G / start));

    colors[0] = glm::vec3(1.0);
    colors[1] = glm::vec3(.3, .3, .85);
    normals[0] = glm::mat3(glm::transpose(glm::inverse(models[0])));
    normals[1] = glm::mat3(glm::transpose(glm::inverse(models[1])));
    isLightSource[0] = true;
    isLightSource[1] = false;
    lightSourceIndices.push_back(0);

    // Mass of sun and planet

#else
    // Uniform distribution for radii, colors, and normal for locations
    std::uniform_real_distribution<float> radii_dist(1, 5);
    std::normal_distribution<float> locations_dist(0, 100);
    std::normal_distribution<float> velocity_dist(0, 5);
    std::uniform_real_distribution<float> light_dist(0, 1);
    std::vector<std::uniform_real_distribution<float>> color_dist = getColorDistribution(
        .1, 1.0,
        .7, .9,
        .8, .9
    );

    GLuint i, j;
    for(i = 0; i < NUM_SPHERES; i++) {
        isLightSource[i] = light_dist(rand_engine) <= LIGHT_FRACTION;
        for(j = 0; j < 3; j++) {
            locations[i][j] = locations_dist(rand_engine);
            velocities[i][j] = velocity_dist(rand_engine);
            if(isLightSource[i]) {
                colors[i][j] = 1;
            }
            else {
                colors[i][j] = color_dist[j](rand_engine);
            }
        }
        radii[i] = isLightSource[i] ? 4 * radii_dist(rand_engine) : radii_dist(rand_engine);
        models[i] = glm::scale(glm::mat4(1.0), glm::vec3(radii[i]));
        models[i] = glm::translate(models[i], locations[i]);
        normals[i] = glm::mat3(glm::transpose(glm::inverse(models[i])));
        masses[i] = 4.0 * pow(radii[i], 3) * M_PI / 3.0 * .8;
        //printNormal<glm::mat3, 3>(normals[i]);
        if(isLightSource[i]) {
            lightSourceIndices.push_back(i);
        }
    } 
    std::cout << lightSourceIndices.size() << std::endl;
#endif

    Shader shader = Shader(VERTEX_PATH, FRAG_PATH);
    // Get replacement map for placeholder strings in shader source
    std::map<std::string, std::string> shaderMacroMap {
        std::make_pair("__NUM_ENTITIES__", std::to_string(NUM_SPHERES)),
        std::make_pair("__NUM_LIGHT_SOURCES__", std::to_string(lightSourceIndices.size()))
    };
    shader.setPlaceholders(shaderMacroMap);
    shader.compileAndLink();

    Sphere sphere(1, glm::vec3(1.0));
    sphere.generateBuffers();
    sphere.bindVertexArray();
    sphere.bindEBO();
    sphere.bindVBO();
    sphere.enableAttributes();
    glBindVertexArray(0);

    // Camera
    glm::mat4 view(1.0); // The view (camera)

    glm::mat4 projection(1.0); // Orthographic or perspective projection
    projection = glm::perspective(glm::radians(FOV), (float) SCR_WIDTH / (float) SCR_HEIGHT, NEAR, FAR);
    //projection = glm::ortho(0.0f, (float) SCR_WIDTH, 0.0f, (float) SCR_HEIGHT, NEAR, FAR);

    // Maintain rotation speed
    float prev = (float) glfwGetTime();
    float next, duration;

    // Reset the 

    // ############### //
    // ## Main Loop ## //
    float accumulator = 0;
    int interval = 0;

    while(!glfwWindowShouldClose(window)) {
        // Adjust camera position and orientation as needed
        camera.updateCameraOrientation(duration);
        camera.updatePosition(duration);
        view = camera.getView();
        
        keyCursorInput.resetDiff();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0, 0.0, 1.0f);

        next = (float) glfwGetTime();
        duration = next - prev;
        prev = next;
        accumulator += duration;
        updateModels(models, normals, locations, velocities, accelerations, masses, radii, isLightSource, lightSourceIndices, duration, accumulator, interval);
        sphere.bindVertexArray();
        shader.setTransform("projection", projection);
        shader.setTransform("view", view);
        shader.setVec3Array("modelColors", colors.data(), colors.size());
        shader.setVec3Array("locations", locations.data(), locations.size());
        shader.setMat4Array("model", models.data(), models.size());
        shader.setMat3Array("normals", normals.data(), normals.size());
        shader.setIntArray("isLightSource", isLightSource.data(), isLightSource.size());
        shader.setIntArray("lightSourceIndices", lightSourceIndices.data(), lightSourceIndices.size());
        shader.use();

        glDrawElementsInstanced(GL_TRIANGLES, sphere.getIndices().size(), GL_UNSIGNED_INT, 0, NUM_SPHERES);

        //std::cout << glGetError() << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
