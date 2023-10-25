#ifndef SSHADER_H
#define SSHADER_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
private:
    int entityCount, lightCount;
    void swapPlaceholders(std::string& vSource, std::string& fSource);
public: 
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
    Shader(const char *vertexPath, const char* fragmentPath, int entityCount, int lightCount);
    
    // No arg constructor. Need to pass shader paths at some point
    Shader();
    // use/activate the shader
    void use();

    // Delete shader
    void remove();

    // utiliy uniform functions
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setTransform(const std::string &name, const glm::mat4& trans);
    void setVec3Array(const std::string& name, const glm::vec3 vec[], int count);
    void setMat4Array(const std::string& name, const glm::mat4 mat[], int count);
    void setMat3Array(const std::string& name, const glm::mat3 mat[], int count);
    void setIntArray(const std::string& name, const GLint array[], int count);

    // The integer ID of the uniform with the given name
    int getUniform(const char *name);
    void setShaderPaths(const char* vertexPath, const char* fragmentPath);

};

#endif
