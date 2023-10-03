#include <iostream>
#include <chrono>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <qtwindow.h>
#include <shader.h>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const GLuint SCR_WIDTH = 1600;
const GLuint SCR_HEIGHT = 1200;
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
const float FAR = 10000;

// Starting camera position
float world_z = -300.0f;
float world_x = 0.0f;
float world_y = -0;

// How fast the figure moves
float z_stride = 1.0f;
float x_stride = 1.0f;

// Figure acceleration
float adj_acceleration = 0.5f;

// Cursor tracking coordinates
double cursor_x = SCR_WIDTH, cursor_y = SCR_HEIGHT;
double diff_x, diff_y;
glm::vec3 camera_axis(0.0, 1.0, 0.0);

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
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
            case GLFW_KEY_ESCAPE:
                // Exit program on Escape
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_W:
                world_z += z_stride;
                z_stride += adj_acceleration;
                break;
            case GLFW_KEY_S:
                world_z -= z_stride;
                z_stride += adj_acceleration;
                break;
            case GLFW_KEY_A:
                world_x += x_stride;
                x_stride += adj_acceleration;
                break;
            case GLFW_KEY_D: 
                world_x -= x_stride;
                x_stride += adj_acceleration;
                break;
        }
    }
    else if(action == GLFW_RELEASE) {
        // Reset velocities
        x_stride = 2.0f;
        z_stride = 2.0f;
    }
}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    diff_x = xpos - cursor_x;
    diff_y = ypos - cursor_y;
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
            vertices.push_back(1.0);
            vertices.push_back(0.0);
            vertices.push_back(0.0);
        }
    }
    return vertices;
}

std::vector<GLuint> generateSphereIndices(std::vector<float>& vertices, GLuint rings, GLuint sectors)
{
    std::vector<GLuint> indices;
    GLuint i;
    // TODO: debug this 
    for(i = 0; i < vertices.size() - sectors - 2 - 10000; i += 3) {
        indices.push_back(i);
        indices.push_back(i + 1);
        indices.push_back(i + sectors - 1);
        indices.push_back(i + 1);
        indices.push_back(i + sectors - 1);
        indices.push_back(i + sectors);
        indices.push_back(i + 1);
        indices.push_back(i + 2);
        indices.push_back(i + sectors);
        indices.push_back(i + 2);
        indices.push_back(i + sectors);
        indices.push_back(i + sectors + 1);
        indices.push_back(i + 2);
        indices.push_back(i + 3);
        indices.push_back(i + sectors + 1);
        indices.push_back(i + 3);
        indices.push_back(i + sectors + 1);
        indices.push_back(i + sectors + 2);
    }
    std::cout << i << std::endl;
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
    GLuint rings = 100, sectors = 100;
    std::vector<float> vertices_vector = generateSphereVertices(500, rings, sectors);
    std::vector<GLuint> indices_vector = generateSphereIndices(vertices_vector, rings, sectors);

    // 3D Diamond
    float size = 20;
    /*float vertices[] = {
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

    float vertices[vertices_vector.size()];
    for(GLuint i = 0; i < vertices_vector.size(); i++) { vertices[i] = vertices_vector[i]; }
    GLuint indices[indices_vector.size()];
    for(GLuint i = 0; i < indices_vector.size(); i++) { indices[i] = indices_vector[i]; }

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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
    glm::mat4 view(1.0);  // The vieeeeew (camera)

    glm::mat4 projection(1.0); // Orthographic or perspective projection
    projection = glm::perspective(glm::radians(85.0f), (float) SCR_WIDTH / (float) SCR_HEIGHT, NEAR, 4000.0f);

    glm::vec3 rot_axis(0.0, 1.0, 0.0);
    rot_axis = glm::normalize(rot_axis);

    // Maintain rotation speed
    float start = (float)glfwGetTime();
    float stop, duration;
    
    // ############### //
    // ## Main Loop ## //
    while(!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1, 0.1, 1.0f);

        stop = (float) glfwGetTime();
        duration = stop - start;
        // Randomly adjust rotation axis

        //local = glm::rotate(local, glm::radians(duration * (ROT_SPEED - 2 * world_z)), rot_axis);
        //adjust_rot_axis(rot_axis);
        
        view = glm::translate(Ident, glm::vec3(world_x, world_y, world_z));
        shader.setTransform("local", local);
        shader.setTransform("model", model);
        shader.setTransform("view", view);
        shader.setTransform("projection", projection);

        // Reset camera deltas
        diff_x = 0; diff_y = 0;

        shader.use();
        start = (float) glfwGetTime();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
