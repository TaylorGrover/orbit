#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <shader.h>
#include <random>

int main()
{
    // Init GLFW
    if(!glfwInit()) {
        std::cerr << "Unable to initialize GLFW\n";
        return -1;
    }

    return 0;
}