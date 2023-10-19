/**
 * A bunch of global variables for the camera transformations, these 
 * should only really be adjusted by key and cursor inputs.
*/
#include <camera.hpp>
#include <iostream>

const float MAX_VELOCITY = 13;
const float MIN_VELOCITY = 5;
const float CAMERA_ROTATION_VELOCITY = 10;

Camera::Camera(Input& input, glm::vec3 position, glm::mat4 orientation) : input(input)
{
    // Starting position of the camera
    Camera::position = position;

    // Initial orientation
    Camera::orientation = orientation;
    glm::mat4 transpose = glm::transpose(orientation);
    camera_right = glm::vec3(transpose * glm::vec4(1.0, 0.0, 0.0, 1.0));
    camera_up = glm::vec3(transpose * glm::vec4(0.0, 1.0, 0.0, 0.0));
    camera_back = glm::cross(camera_right, camera_up);

    // Constants to govern camera speed
    Camera::vel_magnitude = MIN_VELOCITY;
    Camera::acc_magnitude = 20000.0;
    
}

/**
 * The orientation is adjusted following any cursor position changes
*/
void Camera::updateCameraOrientation(float duration)
{
    // Mouse movements affect the value of getDiffX
    orientation = glm::rotate(orientation, (float) glm::radians(input.getDiffX() * CAMERA_ROTATION_VELOCITY * duration), camera_up);
    orientation = glm::rotate(orientation, (float) glm::radians(input.getDiffY() * CAMERA_ROTATION_VELOCITY * duration), camera_right);
    glm::mat4 transpose = glm::transpose(orientation);
    camera_right = glm::vec3(transpose * glm::vec4(1.0, 0.0, 0.0, 1.0));
    camera_up = glm::vec3(transpose * glm::vec4(0.0, 1.0, 0.0, 1.0));
    camera_back = glm::cross(camera_right, camera_up);
}

/**
 * Return the actual matrix used in the transformation
*/
glm::mat4 Camera::getView()
{
    return glm::translate(orientation, position);
}

/**
 * Update the camera position vector following key press and release events
*/
void Camera::updatePosition(float duration)
{
    // This avoids large sequence of if-statements in while loop
    // Not sure if any substantial trade-offs
    bool w, s, a, d, shift, space;
    w = input.isKeyPressed(GLFW_KEY_W);
    s = input.isKeyPressed(GLFW_KEY_S);
    a = input.isKeyPressed(GLFW_KEY_A);
    d = input.isKeyPressed(GLFW_KEY_D);
    shift = input.isKeyPressed(GLFW_KEY_LEFT_SHIFT);
    space = input.isKeyPressed(GLFW_KEY_SPACE);
    position += (float) w * (vel_magnitude * camera_back * duration);
    position -= (float) s * (vel_magnitude * camera_back * duration);
    position += (float) a * (vel_magnitude * camera_right * duration);
    position -= (float) d * (vel_magnitude * camera_right * duration);
    vel_magnitude = ((w | s | a | d) & shift) * acc_magnitude * duration + vel_magnitude;
    if(space) vel_magnitude = MIN_VELOCITY;
}