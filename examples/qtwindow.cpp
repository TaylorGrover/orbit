#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <qtwindow.h>
#include <shader.h>

const GLuint SCR_WIDTH = 800;
const GLuint SCR_HEIGHT = 600;

void configure_window_hints()
{
    // glfw: configure window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

int main()
{
    glfwInit();
    configure_window_hints();

    glfwTerminate();
    return 0;
}
