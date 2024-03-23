/**
 * Handle cursor and keyboard input
*/
#ifndef INPUT_HPP
#define INPUT_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

const int KEYS_COUNT = 349;

// Rotation speed multiplier
const float camera_rot_velocity = .5;

/**
 * @class Input
 * @brief Modify internal state from keystrokes and cursor movements
 *
*/
class Input {
    private:
        bool pressed[KEYS_COUNT];
        bool toggled[KEYS_COUNT];
        double diff_x, diff_y;
        int cursor_x, cursor_y;
    public:
        Input();
        Input(GLuint scr_width, GLuint scr_height);
        void setWidth(GLuint scrWidth);
        void setHeight(GLuint scrHeight);
        void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
        void mouseButtonCallback(GLFWwindow* window, int button, int actions, int mods);
        void setKey(int key);
        void setToggle(int key);
        void unsetKey(int key);
        bool isKeyPressed(int key);
        double getDiffX();
        double getDiffY();
        void resetDiff();
        bool isToggled(int key);
};

#endif
