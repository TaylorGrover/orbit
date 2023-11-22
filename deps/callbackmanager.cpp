#include "callbackmanager.h"

// Cursor and keyboard input handling
Input kcInput(800, 600);

CallbackManager::CallbackManager()
{

}

void CallbackManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    kcInput.keyCallback(window, key, scancode, action, mods);
}


void CallbackManager::cursorPositionCallback(GLFWwindow *window, double xpos, double ypos)
{
    kcInput.cursorPositionCallback(window, xpos, ypos);
}

void CallbackManager::framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    // ensure viewport matches new window dimensions; note that width and
    //   height will be signficantly larger than specified on retina displays
    // Specify location of the lower left corner of the first (x, y), then
    //   the dimensions
    glViewport(0, 0, width, height);
}
