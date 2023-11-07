/**
 * A bunch of global variables for the camera transformations, these 
 * should only really be adjusted by key and cursor inputs.
*/
#include <camera.hpp>
#include <iostream>

const float MAX_VELOCITY = 13;
const float MIN_VELOCITY = .1;
const float MIN_ACCELERATION = 25;
const float CAMERA_ROTATION_VELOCITY = 15;

Camera::Camera(Input& input, glm::vec3 position, glm::mat4 orientation, const float farClip) : input(input)
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
    Camera::acc_magnitude = 25.0;
    Camera::jerk_magnitude = 15.0;
    Camera::farClip = farClip;
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
    bool w, s, a, d, f, c, q, e, shift, space;
    w = input.isKeyPressed(GLFW_KEY_W);
    s = input.isKeyPressed(GLFW_KEY_S);
    a = input.isKeyPressed(GLFW_KEY_A);
    d = input.isKeyPressed(GLFW_KEY_D);
    f = input.isKeyPressed(GLFW_KEY_F);
    c = input.isKeyPressed(GLFW_KEY_C);
    q = input.isKeyPressed(GLFW_KEY_Q);
    e = input.isKeyPressed(GLFW_KEY_E);
    shift = input.isKeyPressed(GLFW_KEY_LEFT_SHIFT);
    space = input.isKeyPressed(GLFW_KEY_SPACE);
    position += (float) w * (vel_magnitude * camera_back * duration);
    position -= (float) s * (vel_magnitude * camera_back * duration);
    position += (float) a * (vel_magnitude * camera_right * duration);
    position -= (float) d * (vel_magnitude * camera_right * duration);
    position += (float) f * (vel_magnitude * -camera_up * duration);
    position += (float) c * (vel_magnitude * camera_up * duration);
    acc_magnitude = ((w || s || a || d || f || c) && shift) * jerk_magnitude * duration + acc_magnitude;
    vel_magnitude = ((w || s || a || d || f || c) && shift) * acc_magnitude * duration + vel_magnitude;
    orientation = glm::rotate(orientation, -glm::radians(10 * CAMERA_ROTATION_VELOCITY * duration * q), camera_back);
    orientation = glm::rotate(orientation, glm::radians(10 * CAMERA_ROTATION_VELOCITY * duration * e), camera_back);
    if(space) {
        acc_magnitude = MIN_ACCELERATION;
        vel_magnitude = MIN_VELOCITY;
    }
    /*// This might be expensive
    if(glm::length(position) > farClip) {
        position = -position;
    }*/
    //std::cout << position[0] << " " << position[1] << " " << position[2] << std::endl;
}

glm::vec3 Camera::getBack() { return camera_back; }
glm::vec3 Camera::getRight() { return camera_right; }
glm::vec3 Camera::getUp() { return camera_up; }
