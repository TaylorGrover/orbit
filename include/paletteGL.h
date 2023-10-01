#ifndef QTWINDOWGL_PROTO_H
#define QTWINDOWGL_PROTO_H

#include <GLFW/glfw3.h>
#include <vector>

// Prototypes
static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void bindTextures(int* texture, const char *img_path);
void configureWindowHints();
void printRGB();
std::vector<float> generateSphereVertices(float radius, GLuint rings, GLuint sectors);
std::vector<GLuint> generateSphereIndices(std::vector<float>& sphereVertices);

#endif
