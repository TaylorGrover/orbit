#include <iostream>
#include <chrono>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <qtwindow.h>
#include <shader.h>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const GLuint SCR_WIDTH = 1800;
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
const float FAR = 100000;

// Starting camera position
glm::vec3 camera_position(0.0, 0.0, -50.0);

// How fast the figure moves
float z_stride = 1.0f;
float x_stride = 1.0f;

// Figure acceleration
glm::vec3 adj_acceleration;

// Cursor tracking coordinates
double cursor_x = SCR_WIDTH / 2, cursor_y = SCR_HEIGHT / 2;
double diff_x, diff_y;
double camera_rot_velocity = .1;

// Camera rotation globals
glm::vec3 camera_right(1.0, 0.0, 0.0);
glm::vec3 camera_up(0.0, 1.0, 0.0);
glm::vec3 camera_back = glm::cross(camera_right, camera_up);
glm::mat4 camera_rot(1.0);
    
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

void updateCameraRotation(glm::mat4& rot, glm::vec3& camera_right, glm::vec3& camera_up)
{
    // Use the global diff_x and diff_y to update the camera axis positions
    glm::mat4 trans = glm::transpose(rot);
    camera_right = glm::vec3(trans * glm::vec4(1.0, 0.0, 0.0, 1.0));
    camera_up = glm::vec3(trans * glm::vec4(0.0, 1.0, 0.0, 1.0));
    camera_back = glm::cross(camera_right, camera_up);
    /*int i;
    std::cout << "Right: ";
    for(i = 0; i < 3; i++) {std::cout << camera_right[i] << " ";}
    std::cout << "Up: ";
    for(i = 0; i < 3; i++) {std::cout << camera_up[i] << " ";}
    std::cout << "Back: ";
    for(i = 0; i < 3; i++) {std::cout << camera_back[i] << " ";}
    std::cout << "Dot: " << glm::dot(camera_right, camera_up);
    std::cout << std::endl; */
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        adj_acceleration = glm::normalize(camera_back);
        if(key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if(key == GLFW_KEY_W) {
            camera_position += camera_back;
            camera_back += adj_acceleration;
            /*std::cout << "Current position: ";
            for(int i = 0; i < 3; i++) {std::cout << camera_position[i] << " ";}
            std::cout << std::endl; 
            */
        }
        if(key == GLFW_KEY_S) {
            camera_position -= camera_back;
            camera_back += adj_acceleration;
        }
        if(key == GLFW_KEY_A) {
            // TODO
        }
        if(key == GLFW_KEY_D) {
            // TODO
        }
    }
    else if(action == GLFW_RELEASE) {
        // Reset velocities
        camera_back = glm::cross(camera_right, camera_up);
    }
}

static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    //std::cout << "cursor_x: " << cursor_x <<"\tcursor_y: " << cursor_y << "\txpos: " << xpos << "\typos: " << ypos << std::endl;
    diff_x = xpos - cursor_x;
    diff_y = ypos - cursor_y;
    cursor_x = xpos;
    cursor_y = ypos;
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
    for(i = 0; i < vertices.size() - sectors - 2; i += 3) {
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
    GLuint rings = 100, sectors = 100;
    std::vector<float> vertices_vector = generateSphereVertices(500, rings, sectors);
    std::vector<GLuint> indices_vector = generateSphereIndices(vertices_vector, rings, sectors);

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
    projection = glm::perspective(glm::radians(FOV), (float) SCR_WIDTH / (float) SCR_HEIGHT, NEAR, FAR);

    glm::vec3 rot_axis(0.0, 1.0, 0.0);
    
    // Maintain rotation speed
    //float start = (float)glfwGetTime();
    //float stop, duration;

    // Reset the 

    // ############### //
    // ## Main Loop ## //

    while(!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.1f, 0.1, 0.1, 1.0f);

        //stop = (float) glfwGetTime();
        //duration = stop - start;

        // Rotate the camera view to the current position then perform any 
        //   updates on the rotation matrix
        view = glm::rotate(camera_rot, (float) glm::radians(diff_x * camera_rot_velocity), camera_up);
        view = glm::rotate(view, (float) glm::radians(diff_y * camera_rot_velocity), camera_right);
        camera_rot = view;
        updateCameraRotation(camera_rot, camera_right, camera_up);
        view = glm::translate(camera_rot, camera_position);
        
        diff_x = 0;
        diff_y = 0;
        
        shader.setTransform("local", local);
        shader.setTransform("model", model);
        shader.setTransform("view", view);
        shader.setTransform("projection", projection);

        shader.use();
        //start = (float) glfwGetTime();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
