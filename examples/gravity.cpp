#include <iostream>
#include <chrono>
#include <callbackmanager.h>
#include <cmath>
#include <camera.hpp>
#include <entity.hpp>
#include <spheremanager.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <input.hpp>
#include <map>
#include <parametermanager.h>
#include <qt5/QtWidgets/QApplication>
#include <qt5/QtWidgets/QDialog>
#include <qt5/QtWidgets/QVBoxLayout>
#include <qt5/QtCore/QObject>
#include <shader.h>
#include <SettingsDialog.h>
#include <string.h>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define DEBUG_OFF


const GLuint SCR_WIDTH = 2400;
const GLuint SCR_HEIGHT = 1600;

const char *WINDOW_TITLE = "Gravity";
const char *VERTEX_PATH = "shaders/shader.vs";
const char *FRAG_PATH = "shaders/shader.fs";

// Gravitational constant (scaled by 10^18) N * m^2 / kg^2
// FOV
const float FOV = 85;

// Near and far clipping plane distances
const float NEAR = 0.01f;
const float FAR = 5000;

// Starting camera position
glm::vec3 cameraPosition(0.0, 0.0, 0.0);
glm::mat4 cameraOrientation = glm::rotate(glm::mat4(1.0), glm::radians(0.0f), glm::vec3(0.0, 1.0, 0.0));

// Cursor and keyboard input handling
Input keyCursorInput(SCR_WIDTH, SCR_HEIGHT);

// Camera rotation globals
Camera camera(keyCursorInput, cameraPosition, cameraOrientation);

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

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    keyCursorInput.keyCallback(window, key, scancode, action, mods);
}

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    keyCursorInput.cursorPositionCallback(window, xpos, ypos);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // ensure viewport matches new window dimensions; note that width and 
    //   height will be signficantly larger than specified on retina displays
    // Specify location of the lower left corner of the first (x, y), then
    //   the dimensions
    glViewport(0, 0, width, height);
}

int main(int argc, char* argv[])
{
    // Parameter management
    ParameterManager& paramManager = ParameterManager::getInstance();
    // Set up QApplication
    QApplication app(argc, argv);
    SettingsDialog settingsDialog(paramManager);
    settingsDialog.exec();

    if(settingsDialog.result() == QDialog::Rejected) {
        return 0;
    }

    if(!glfwInit()) {
        std::cerr << "Unable to initialize GLFW\n";
        return -1;
    }
    
    // glfwWindowHint calls
    configureWindowHints();

    GLFWwindow* window;
    if(paramManager.getFullscreenChecked()) {
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL);
    }
    else {
        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, WINDOW_TITLE, NULL, NULL);
    }
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

    // Depth-testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    // Wireframe mode (must be called after gladLoadGLLoader
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    SphereManager sphereManager(VERTEX_PATH, FRAG_PATH);

    //const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine rand_engine(paramManager.getRandSeed());
    sphereManager.initializeSpheres(rand_engine, paramManager);

    // Camera
    glm::mat4 view(1.0); // The view (camera)

    glm::mat4 projection(1.0); // Orthographic or perspective projection
    projection = glm::perspective(glm::radians(FOV), (float) SCR_WIDTH / (float) SCR_HEIGHT, NEAR, FAR);
    //projection = glm::ortho(0.0f, (float) SCR_WIDTH, 0.0f, (float) SCR_HEIGHT, NEAR, FAR);

    // Maintain rotation speed
    float prev = (float) glfwGetTime();
    float next, duration;

    // ############### //
    // ## Main Loop ## //
    float accumulator = 0;

    while(!glfwWindowShouldClose(window)) {
        // Adjust camera position and orientation as needed
        camera.updateCameraOrientation(duration);
        camera.updatePosition(duration);
        view = camera.getView();
        
        keyCursorInput.resetDiff();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.01, 0.01, 1.0f);

        next = (float) glfwGetTime();
        duration = next - prev;
        prev = next;
        if(keyCursorInput.isToggled(GLFW_KEY_Z)) {
            // Increase total elapsed time if Z toggled
            accumulator += duration;
            sphereManager.gravitateSerialAbsorbCollisions(duration);
        }
        sphereManager.bindVertexArray();
        sphereManager.setShaderUniforms(view, projection);
        sphereManager.useShader();

        glDrawElementsInstanced(GL_TRIANGLES, sphereManager.getIndices().size(), GL_UNSIGNED_INT, 0, sphereManager.getSphereCount());

        //std::cout << glGetError() << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
