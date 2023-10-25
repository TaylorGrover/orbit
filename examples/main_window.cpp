#include <iostream>
#include <chrono>
#include <cmath>
#include <camera.hpp>
#include <entity.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <input.hpp>
#include <qtwindow.h>
#include <shader.h>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define DEBUG_ON

const GLuint SCR_WIDTH = 2400;
const GLuint SCR_HEIGHT = 1600;

#ifndef DEBUG_ON
const GLuint NUM_SPHERES = 1 << 3;
#else
const GLuint NUM_SPHERES = 2;
#endif

const char *WINDOW_TITLE = "Orbit";
const char *VERTEX_PATH = "shaders/shader.vs";
const char *FRAG_PATH = "shaders/shader.fs";

// Fraction of entities that are lights
const float LIGHT_FRACTION = .1;

// Gravitational constant (scaled by 10^18) N * m^2 / kg^2
const float G = 6.64728e9;

// Distance from earth to Sun (apoapsis)
const float ES_DIST = 147e6;


// Mass of Earth (scaled by 1/10^9)

// Aspect ratio
const float ASPECT_RATIO = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);

// Rotation speed (degrees/second)
const float ROT_SPEED = 3;

// FOV
const float FOV = 85;

// Near and far clipping plane distances
const float NEAR = 0.01f;
const float FAR = 1000;

// Starting camera position
glm::vec3 camera_position(0.0, 0.0, 0.0);

// How fast the figure moves
float z_stride = 1.0f;
float x_stride = 1.0f;

// Figure acceleration
float acc_magnitude = 2.0;

// Cursor tracking coordinates
double cursor_x = SCR_WIDTH / 2, cursor_y = SCR_HEIGHT / 2;
double diff_x, diff_y;

// The key input selection
const int KEY_COUNT = 349;
bool selected[KEY_COUNT];
Input keyCursorInput(SCR_WIDTH, SCR_HEIGHT);


// Camera rotation globals
#ifndef DEBUG_ON
Camera camera(keyCursorInput, camera_position, glm::mat4(1.0), FAR);
#else
Camera camera(keyCursorInput, glm::vec3(0, 0, 0), glm::rotate(glm::mat4(1.0), glm::radians(-55.0f), glm::vec3(0.0, 1.0, 0.0)), FAR);
#endif

    
// Normal distribution
const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine rand_engine(seed);
std::normal_distribution<float> norm(0, 2);

std::vector<std::uniform_real_distribution<float>> getColorDistribution(float rlow, float rhigh, float glow, float ghigh, float blow, float bhigh)
{
    std::vector<std::uniform_real_distribution<float>> color_dist;
    color_dist.push_back(std::uniform_real_distribution<float>(rlow, rhigh));
    color_dist.push_back(std::uniform_real_distribution<float>(glow, ghigh));
    color_dist.push_back(std::uniform_real_distribution<float>(blow, bhigh));
    return color_dist;
}

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
void updateModels(glm::mat4 models[], int count, float duration)
{
    // Rescale all the models
    for(int i = 0; i < count; i++) {
        models[i] = glm::rotate(models[i], glm::radians(10.0f * duration), glm::vec3(0.0, 1.0, 0.0));
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

    // Generate vertices for sphere
    /*GLuint rings = 200, sectors = 200;
    std::vector<float> vertices = generateSphereVertices(5000, rings, sectors);
    std::vector<GLuint> indices = generateSphereIndices(vertices, rings, sectors);

    // 3D Diamond
    float size = 20;
    float vertices[] = {
         0.0f,   size,  0.0f,   1.0f, 0.0f, 0.0f, // top
         0.0f,   0.0f,  size/4, 0.5f, 0.5f, 0.0f, // front
         size/2, 0.0f,  0.0f,   0.5f, 0.4f, 0.1f, // right
        -size/2, 0.0f,  0.0f,   0.5f, 0.4f, 0.1f, // left
         0.0f,   0.0f, -size/4, 0.5f, 0.5f, 0.0f, // back
         0.0f,  -size,  0.0f,   4.0f, 3.0f, 3.0f, // bottom
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 1, 3,
        0, 4, 2,
        0, 4, 3,
        5, 1, 2,
        5, 1, 3,
        5, 4, 2,
        5, 4, 3,
    }; 


    // Setup textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char *data = stbi_load("deps/img/wall.jpg", &width, &height, &nrChannels, 0);
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    if(data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data); */

    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));


    glm::mat4 models[NUM_SPHERES];
    glm::mat3 normals[NUM_SPHERES];
    glm::vec3 locations[NUM_SPHERES];
    std::fill(models, models + NUM_SPHERES, glm::mat4(1.0));
    glm::vec3 colors[NUM_SPHERES];
    // Essentially a boolean array
    GLint isLightSource[NUM_SPHERES];
    std::vector<GLint> lightSourceIndices;

#ifdef DEBUG_ON
    // For debugging
    models[0] = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(5)), glm::vec3(0, 0, 0));
    models[1] = glm::translate(glm::scale(glm::mat4(1.0), glm::vec3(1)), glm::vec3(15.0, 0.0, 0.0));
    colors[0] = glm::vec3(1.0);
    colors[1] = glm::vec3(.75, .75, .85);
    normals[0] = glm::mat3(glm::transpose(glm::inverse(models[0])));
    normals[1] = glm::mat3(glm::transpose(glm::inverse(models[1])));
    isLightSource[0] = true;
    isLightSource[1] = false;
    lightSourceIndices.push_back(0);
#else
    // Uniform distribution for radii, colors, and normal for locations
    std::uniform_real_distribution<float> radii_dist(1, 5);
    std::normal_distribution<float> locations_dist(0, 50);
    std::uniform_real_distribution<float> light_dist(0, 1);
    std::vector<std::uniform_real_distribution<float>> color_dist = getColorDistribution(
        .1, .3,
        .3, .4,
        .7, .8
    );

    GLuint i;
    for(i = 0; i < NUM_SPHERES; i++) {
        isLightSource[i] = light_dist(rand_engine) <= LIGHT_FRACTION;
        for(GLuint j = 0; j < 3; j++) {
            locations[i][j] = locations_dist(rand_engine);
            if(isLightSource[i]) {
                colors[i][j] = 1;
            }
            else {
                colors[i][j] = color_dist[j](rand_engine);
            }
        }
        models[i] = glm::translate(glm::mat4(1.0), locations[i]);
        models[i] = glm::scale(models[i], glm::vec3(radii_dist(rand_engine)));
        normals[i] = glm::transpose(glm::inverse(glm::mat3(models[i])));
        if(isLightSource[i]) {
            lightSourceIndices.push_back(i);
        }
    } 
    std::cout << lightSourceIndices.size() << std::endl;
#endif

    Shader shader = Shader(VERTEX_PATH, FRAG_PATH, NUM_SPHERES, lightSourceIndices.size()); 
    Sphere sphere(1, glm::vec3(1.0), shader);
    sphere.setPosition(glm::vec3(0.0));
    sphere.generateBuffers();
    sphere.bindVertexArray();
    sphere.bindEBO();
    sphere.bindVBO();
    sphere.enableAttributes();
    glBindVertexArray(0);

    shader.setVec3Array("modelColors", colors, NUM_SPHERES);
    shader.setMat4Array("model", models, NUM_SPHERES);
    shader.setMat3Array("normalMatrices", normals, NUM_SPHERES);
    shader.setIntArray("isLightSource", isLightSource, NUM_SPHERES);
    shader.setIntArray("lightSourceIndices", lightSourceIndices.data(), lightSourceIndices.size());

    // Camera
    glm::mat4 view(1.0); // The view (camera)

    glm::mat4 projection(1.0); // Orthographic or perspective projection
    projection = glm::perspective(glm::radians(FOV), (float) SCR_WIDTH / (float) SCR_HEIGHT, NEAR, FAR);
    //projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, NEAR, FAR);

    glm::vec3 rot_axis(0.0, 1.0, 0.0);

    
    // Maintain rotation speed
    float prev = (float) glfwGetTime();
    float next, duration;

    // Reset the 

    // ############### //
    // ## Main Loop ## //
    float accumulator;

    while(!glfwWindowShouldClose(window)) {
        // Rotate the camera view to the current position then perform any 
        //   updates on the rotation matrix
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

        //updateModels(models, NUM_SPHERES, duration);
        sphere.bindVertexArray();
        sphere.rotateByDegrees(2 * duration, rot_axis);
        shader.setTransform("projection", projection);
        shader.setTransform("view", view);
        shader.setVec3Array("modelColors", colors, NUM_SPHERES);
        shader.setMat4Array("model", models, NUM_SPHERES);
        shader.setMat3Array("normalMatrices", normals, NUM_SPHERES);
        shader.setIntArray("isLightSource", isLightSource, NUM_SPHERES);
        shader.setIntArray("lightSourceIndices", lightSourceIndices.data(), lightSourceIndices.size());
        shader.use();

        glDrawElementsInstanced(GL_TRIANGLES, sphere.getIndices().size(), GL_UNSIGNED_INT, 0, NUM_SPHERES);

        //std::cout << glGetError() << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    sphere.deleteBuffers();
    glfwTerminate();
    return 0;
}
