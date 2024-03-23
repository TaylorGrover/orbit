#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <input.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera {
    private: 
        glm::vec3 camera_right;
        glm::vec3 camera_up;
        glm::vec3 camera_back;

        glm::mat4 orientation;
        glm::vec3 position;
        glm::vec3 velocity;


        float vel_magnitude;
        float acc_magnitude;
        float jerk_magnitude;

        // Keyboard and cursor input management
        Input& input;

    public:
        Camera(Input& input, glm::vec3 position, glm::mat4 orientation);
        void updateCameraOrientation(float duration);
        void updatePositionGodMode(float duration);
        void updatePositionRegular(float duration);
        void updatePositionExternal(glm::vec3 pos);
        void updateVelocityExternal(glm::vec3 vel);
        glm::vec3 getPosition();
        glm::vec3 getVelocity();
        glm::vec3 getBack();
        glm::vec3 getRight();
        glm::vec3 getUp();
        glm::mat4 getView();
};
#endif
