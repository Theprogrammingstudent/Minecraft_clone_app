#pragma once
#include <string>
#include <glm/glm.hpp>          // ADD THIS
#include <glad/glad.h>

class Shader
{
public:
    unsigned int ID;
    Shader(const char* vertexSrc, const char* fragmentSrc);
    void use();

    // ADD THESE THREE
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, glm::vec3 value);
    void setMat4(const std::string& name, glm::mat4 value);

private:
    void checkCompileErrors(unsigned int shader, std::string type);
};