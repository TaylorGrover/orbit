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


        float vel_magnitude;
        float acc_magnitude;

        // Keyboard and cursor input management

    public:
        Input input;
        Camera(Input& input, glm::vec3 position, glm::mat4 orientation);
        void updateCameraOrientation();
        void updatePosition();
        glm::mat4 getView();
};
#endif