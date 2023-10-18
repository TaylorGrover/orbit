#include <iostream>
#include <chrono>
#include <cmath>
#include <camera.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <input.hpp>
#include <qtwindow.h>
#include <shader.h>
#include <sphere.hpp>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const GLuint SCR_WIDTH = 2400;
const GLuint SCR_HEIGHT = 1600;
const GLuint NUM_SPHERES = 100;
const char *WINDOW_TITLE = "Orbit";

// Aspect ratio
const float ASPECT_RATIO = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);

// Rotation speed (degrees/second)
const float ROT_SPEED = 3;

// FOV
const float FOV = 85;

// Near and far clipping plane distances
const float NEAR = 0.01f;
const float FAR = 1100000;

// Starting camera position
glm::vec3 camera_position(0.0, 0.0, -500.0);

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
Camera camera(keyCursorInput, camera_position, glm::mat4(1.0));
    
// Normal distribution
const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine rand_engine(seed);
std::normal_distribution<float> norm(0, 2);

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

/**
 * Rotate the local rotation matrix by some small random angle about some 
 *   random vector.
*/
void adjust_rot_axis(glm::vec3& rot_axis)
{
    for(int i = 0; i < 3; i++) { rot_axis[i] += norm(rand_engine); }
    rot_axis = glm::normalize(rot_axis);
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

int main()
{
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

    Shader shader = Shader("shaders/shader.vs", "shaders/shader.fs"); 
    Sphere sphere(1200, glm::vec3(1.0), shader);
    sphere.setPosition(glm::vec3(0.0));
    sphere.generateBuffers();
    sphere.bindVertexArray();
    sphere.bindEBO();
    sphere.bindVBO();
    sphere.enableAttributes();
    glBindVertexArray(0);
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));


    glm::mat4 models[NUM_SPHERES];
    glm::vec3 locations[NUM_SPHERES];
    std::fill(models, models + NUM_SPHERES, glm::mat4(1.0));
    glm::vec3 colors[NUM_SPHERES];
    // Uniform distribution for radii and normal for spatial locations
    std::uniform_real_distribution<float> radii_dist(10, 120);
    std::normal_distribution<float> locations_dist(0, 70000);
    std::vector<std::uniform_real_distribution<float>> color_dist;
    color_dist.push_back(std::uniform_real_distribution<float>(.7, .8));
    color_dist.push_back(std::uniform_real_distribution<float>(.4, .45));
    color_dist.push_back(std::uniform_real_distribution<float>(.2, .3));
    GLuint i;
    for(i = 0; i < NUM_SPHERES; i++) {
        for(GLuint j = 0; j < 3; j++) {
            locations[i][j] = locations_dist(rand_engine);
            colors[i][j] = color_dist[j](rand_engine);
        }
        models[i] = glm::translate(glm::mat4(1.0), locations[i]);
    }

    shader.setVec3s("modelColors", colors, NUM_SPHERES);
    shader.setMat4Array("model", models, NUM_SPHERES);

    // Camera
    glm::mat4 view(1.0); // The view (camera)

    glm::mat4 projection(1.0); // Orthographic or perspective projection
    projection = glm::perspective(glm::radians(FOV), (float) SCR_WIDTH / (float) SCR_HEIGHT, NEAR, FAR);

    glm::vec3 rot_axis(0.0, 1.0, 0.0);

    
    // Maintain rotation speed
    float start = (float)glfwGetTime();
    float stop, duration;

    // Reset the 

    // ############### //
    // ## Main Loop ## //

    while(!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0, 0.0, 1.0f);

        stop = (float) glfwGetTime();
        duration = stop - start;

        // Rotate the camera view to the current position then perform any 
        //   updates on the rotation matrix
        camera.updateCameraOrientation(duration);
        camera.updatePosition(duration);
        view = camera.getView();
        
        keyCursorInput.resetDiff();

        //model = glm::rotate(model, glm::radians(ROT_SPEED * duration), rot_axis);
        
        // Place in a loop for each object 
        //shader.setTransform("local", local);
        sphere.bindVertexArray();
        sphere.rotateByDegrees(2 * duration, rot_axis);
        shader.setTransform("projection", projection);
        shader.setTransform("view", view);
        shader.setMat4Array("model", models, NUM_SPHERES);
        shader.setVec3s("modelColors", colors, NUM_SPHERES);
        shader.use();
        start = (float) glfwGetTime();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.getEBO());
        glDrawElementsInstanced(GL_TRIANGLES, sphere.getIndices().size(), GL_UNSIGNED_INT, 0, NUM_SPHERES);

        //glGetError();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    sphere.deleteBuffers();
    glfwTerminate();
    return 0;
}
