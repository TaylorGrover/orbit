#ifndef QTWINDOWGL_PROTO_H
#define QTWINDOWGL_PROTO_H
// Prototypes
#include <GLFW/glfw3.h>

static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void bind_textures(int* texture, const char *img_path);
void configure_window_hints();

#endif
