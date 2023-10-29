#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <qtwindow.h>
#include <shader.h>
#include <stdio.h>

#define INC_COLOR(c) ((c + 1) % 256)
#define DEC_COLOR(c) ((c - 1) % 256)

const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;
const char *WINDOW_TITLE = "Orbit";
const char *VERTEX_PATH = "shaders/palette.vs";
const char *FRAG_PATH = "shaders/palette.fs";

// Global shader and rgb values
GLuint r = 0, g = 0, b = 0;


void configureWindowHints()
{
    // glfw: configure window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    // Using GLFW_VERSION_MINOR makes the window uncreatable for some reason, so
    //   a hardcoded 3 is used
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void printRGB()
{
    std::cout << "(" << r << ", " << g << ", " << b << ")\r";
    fflush(stdout);
}

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
            case GLFW_KEY_ESCAPE:
                // Exit program on Escape
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case GLFW_KEY_R:
                r = INC_COLOR(r);
                printRGB();
                break;
            case GLFW_KEY_G:
                g = INC_COLOR(g);
                printRGB();
                break;
            case GLFW_KEY_B:
                b = INC_COLOR(b);
                printRGB();
                break;
            case GLFW_KEY_E:
                r = DEC_COLOR(r);
                printRGB();
                break;
            case GLFW_KEY_F:
                g = DEC_COLOR(g);
                printRGB();
                break;
            case GLFW_KEY_V:
                b = DEC_COLOR(b);
                printRGB();
                break;
        }
    }
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
    if(window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Register events callback
    glfwSetKeyCallback(window, keyCallback);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // glad: load all OpenGL function pointers
    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Wireframe mode (must be called after gladLoadGLLoader
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float vertices[] = {
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top left
         1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // top right
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f  // bottom right
    };

    GLuint indices[] = {
        0, 1, 2,
        1, 2, 3
    }; 

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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    Shader shader(VERTEX_PATH, FRAG_PATH);
    shader.use();
    
    // ############### //
    // ## Main Loop ## //
    while(!glfwWindowShouldClose(window)) {

        glClearColor(0.5f, 0.5, 0.5, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.setFloat("roff", (float) r / 255.0);
        shader.setFloat("goff", (float) g / 255.0);
        shader.setFloat("boff", (float) b / 255.0);
        shader.use();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
