#ifndef QTWINDOWGL_PROTO_H
#define QTWINDOWGL_PROTO_H
// Prototypes
#include <GLFW/glfw3.h>

static void framebufer_size_callback(GLFWwindow* window, int width, int height);
void bind_textures(int* texture, const char *img_path);
void configure_window_hints();

#endif
