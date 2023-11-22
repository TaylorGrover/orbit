#ifndef CALLBACKMANAGER_H
#define CALLBACKMANAGER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera.hpp>
#include <input.hpp>


class CallbackManager
{
public:
    CallbackManager();
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};

#endif // CALLBACKMANAGER_H
