#include <input.hpp>
#include <iostream>

extern const GLuint SCR_WIDTH, SCR_HEIGHT;

/**
 * 
*/
Input::Input()
{
    cursor_x = 800;
    cursor_y = 600;
}

Input::Input(GLuint scr_width, GLuint scr_height)
{
    cursor_x = scr_width / 2;
    cursor_y = scr_height / 2;
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action == GLFW_PRESS || action == GLFW_REPEAT) {
        setKey(key);
        if(key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
    else if(action == GLFW_RELEASE) {
        unsetKey(key);
    }
}

void Input::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    diff_x = xpos - cursor_x;
    diff_y = ypos - cursor_y;
    cursor_x = xpos;
    cursor_y = ypos;
}

void Input::setKey(int key) 
{
    Input::pressed[key] = true;
}

void Input::unsetKey(int key) 
{
    Input::pressed[key] = false;
}

bool Input::isKeyPressed(int key)
{
    return pressed[key];
}

/**
 * Return the difference between the current and previous coordinates
*/
double Input::getDiffX()
{
    return diff_x;
}

double Input::getDiffY()
{
    return diff_y;
}

/**
 * Set the diff_x and diff_y variables to 0
*/
void Input::resetDiff()
{
    diff_x = 0;
    diff_y = 0;
}