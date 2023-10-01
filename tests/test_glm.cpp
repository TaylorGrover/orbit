#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

void print_vec4(glm::vec4 vec);

class GLMTestSuite {
    public:
        void rotateX90() {

        }
};

int main()
{
    glm::vec4 vec(2.0f, 3.0f, 0.0f, 1.0f);

    // Initialize the matrix diagonals 
    glm::mat4 trans = glm::mat4(1.0f);
    
    trans = glm::rotate(trans, -glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
    vec = trans * vec;
    print_vec4(vec);
    glm::vec4 expected(0.0, 3.0, 2.0, 1.0);
    return 0;
}

void print_vec4(glm::vec4 vec)
{
    std::cout << "[";
    int i;
    for(i = 0; i < vec.length() - 1; i++) {
        std::cout << vec[i] << ", ";
    }
    std::cout << vec[i] << "]" << std::endl;
}

