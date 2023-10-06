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
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const GLuint SCR_WIDTH = 2400;
const GLuint SCR_HEIGHT = 1600;
const GLuint MAXDIST = 1000;
const char *WINDOW_TITLE = "Orbit";

// Aspect ratio
const float ASPECT_RATIO = SCR_WIDTH / SCR_HEIGHT;

// Rotation speed (degrees/second)
const float ROT_SPEED = 0;

// FOV
const float FOV = 85;

// Near and far clipping plane distances
const float NEAR = 0.01f;
const float FAR = 100000;

// Starting camera position
glm::vec3 camera_position(0.0, 0.0, -50.0);

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
    camera.input.keyCallback(window, key, scancode, action, mods);
}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    camera.input.cursorPositionCallback(window, xpos, ypos);
}

static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // ensure viewport matches new window dimensions; note that width and 
    //   height will be signficantly larger than specified on retina displays
    // Specify location of the lower left corner of the first (x, y), then
    //   the dimensions
    glViewport(0, 0, width, height);
}

std::vector<float> generateSphereVertices(float radius, GLuint rings, GLuint sectors)
{
    std::vector<float> vertices;
    const float PI = 3.14159265359;

    if(rings == 0 || sectors == 0) {
        std::cerr << "One of rings or sectors is 0. Defaulting both to 100\n";
        rings = 100; sectors = 100;
    }

    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);

    for (GLuint r = 0; r < rings; r++) {
        for(GLuint s = 0; s < sectors; s++) {
            float const y = sin(-PI / 2 + PI * r * R);
            float const x = cos(2 * PI * s * S) * sin(PI * R * r);
            float const z = sin(2 * PI * s * S) * sin(PI * R * r);

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
            vertices.push_back(10.0 / 255);
            vertices.push_back(148.0 / 255);
            vertices.push_back(228.0 / 255);
        }
    }
    return vertices;
}

std::vector<GLuint> generateSphereIndices(std::vector<float>& vertices, GLuint rings, GLuint sectors)
{
    std::vector<GLuint> indices;
    GLuint i;
    // TODO: debug this 
    for(i = 0; i < vertices.size() - sectors - 4; i += 3) {
        for(int j = 1; j <= 3; j++) {
            indices.push_back(i + j - 1);
            indices.push_back(i + j);
            indices.push_back(i + sectors + j - 1);

            indices.push_back(i + j);
            indices.push_back(i + sectors + j - 1);
            indices.push_back(i + sectors + j);
        }
    }
    return indices;
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

    // Capture mouse
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
    GLuint rings = 200, sectors = 200;
    std::vector<float> vertices = generateSphereVertices(5000, rings, sectors);
    std::vector<GLuint> indices = generateSphereIndices(vertices, rings, sectors);

    // 3D Diamond
    /*float size = 20;
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
    }; */


    // Setup textures
    /*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
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

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));

    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));

    Shader shader = Shader("shaders/shader.vs", "shaders/shader.fs");

    glm::mat4 Ident(1.0);
    glm::mat4 local(1.0); // Local transformation;
    glm::mat4 model(1.0); // Model transformation; where is it in the world?
    
    // Camera
    glm::mat4 view(1.0);  // The vieeeeew (camera)

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
        glClearColor(0.1f, 0.1, 0.1, 1.0f);

        stop = (float) glfwGetTime();
        duration = stop - start;

        // Rotate the camera view to the current position then perform any 
        //   updates on the rotation matrix
        camera.updateCameraOrientation();
        camera.updatePosition();
        view = camera.getView();
        
        camera.input.resetDiff();

        local = glm::rotate(local, glm::radians(ROT_SPEED * duration), rot_axis);
        
        // Place in a loop for each object 
        shader.setTransform("local", local);
        shader.setTransform("model", model);
        shader.setTransform("view", view);
        shader.setTransform("projection", projection);

        shader.use();
        start = (float) glfwGetTime();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
