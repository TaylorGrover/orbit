#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <input.hpp>

const GLuint SCR_WIDTH = 1600;
const GLuint SCR_HEIGHT = 1200;

int test_key_press()
{
    Input input(SCR_WIDTH, SCR_HEIGHT);
    input.keyCallback(nullptr, GLFW_KEY_W, 0, GLFW_PRESS, 0);
    if(!input.isKeyPressed(GLFW_KEY_W)) {
        return 1;
    }
    return 0;
}

int main()
{
    int result;
    result = test_key_press();

    if(result != 0)
        return result;
    return 0;
}
