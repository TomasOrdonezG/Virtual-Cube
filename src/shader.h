#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <stdio.h>

class Shader
{
public:

    GLuint ID;

    Shader() {}

    Shader(const char *vertexShaderSource, const char *fragmentShaderSource)
    {
        // Compile and set up the shaders
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        vertexShader = compileShader(vertexShader, vertexShaderSource);

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        fragmentShader = compileShader(fragmentShader, fragmentShaderSource);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        // Check for linking errors
        GLint success;
        glGetProgramiv(ID, GL_LINK_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetProgramInfoLog(ID, sizeof(infoLog), NULL, infoLog);
            fprintf(stderr, "ERROR::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
        }

        // Clean up shaders as they're no longer needed
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

private:

    GLuint compileShader(GLuint shader, const char* shaderSource)
    {
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        // Check for compilation errors
        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER::COMPILATION_FAILED\n%s\n", infoLog);
        }
        
        return shader;
    }

};

#endif
