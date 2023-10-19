#include "shader.h"

/**
 * Swap the placeholder strings in the vertex and fragment sources 
 *   for the entity and light source counts
*/
void Shader::swapPlaceholders(std::string& vSource, std::string& fSource)
{
    std::string replaceNumEntities = "__NUM_ENTITIES__";
    std::string replaceNumLights = "__NUM_LIGHT_SOURCES__";
    size_t startIndex = vSource.find(replaceNumEntities, 0);
    vSource.replace(startIndex, replaceNumEntities.size(), std::to_string(entityCount));
    startIndex = fSource.find(replaceNumEntities, 0);
    fSource.replace(startIndex, replaceNumEntities.size(), std::to_string(entityCount));
    startIndex = fSource.find(replaceNumLights, 0);
    fSource.replace(startIndex, replaceNumLights.size(), std::to_string(lightCount));
}

Shader::Shader() {}

Shader::Shader(const char* vertexPath, const char* fragmentPath, int entityCount, int lightCount) : entityCount(entityCount), lightCount(lightCount)
{
    setShaderPaths(vertexPath, fragmentPath);
}

void Shader::setShaderPaths(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        
        // convert stream into string
        vertexCode = vShaderStream.str();

        fragmentCode = fShaderStream.str();

        // Replace __NUM_ENTITIES__ with the given value
        swapPlaceholders(vertexCode, fragmentCode);

    } catch(std::ifstream::failure& err) {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // similar for Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // shader program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR:SHADER:PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // delete shaders; they're linked into our program and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() 
{
    glUseProgram(this->ID);
}

void Shader::remove()
{
    glDeleteProgram(this->ID);
}

void Shader::setTransform(const std::string &name, const glm::mat4& trans)
{
    unsigned int uniformID = glGetUniformLocation(this->ID, name.c_str());
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(trans));
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(this->ID, name.c_str()), (int) value);
}

void Shader::setInt(const std::string &name, int value) const
{
    uint uniformID = glGetUniformLocation(this->ID, name.c_str());
    glUniform1i(uniformID, value);
}

void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

int Shader::getUniform(const char *name)
{
    return glGetUniformLocation(this->ID, name);
}

void Shader::setVec3Array(const std::string& name, const glm::vec3 vec[], int count)
{
    if(count > 0) {
        uint uniformID = glGetUniformLocation(this->ID, name.c_str());
        glUniform3fv(uniformID, count, glm::value_ptr(vec[0]));
    }
}

void Shader::setMat4Array(const std::string& name, const glm::mat4 mat[], int count)
{
    if(count > 0) {
        uint uniformID = glGetUniformLocation(this->ID, name.c_str());
        glUniformMatrix4fv(uniformID, count, GL_FALSE, glm::value_ptr(mat[0]));
    }
}

void Shader::setIntArray(const std::string& name, const GLint array[], int count)
{
    if(count > 0) {
        uint uniformID = glGetUniformLocation(this->ID, name.c_str());
        glUniform1iv(uniformID, count, array);
    }
}