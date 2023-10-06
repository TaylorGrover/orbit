#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <input.hpp>

const GLuint SCR_WIDTH = 1600;
const GLuint SCR_HEIGHT = 1200;

int main()
{
    Input input(SCR_WIDTH, SCR_HEIGHT);
    return 0;
}