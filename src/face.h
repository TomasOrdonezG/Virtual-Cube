#ifndef FACE_H
#define FACE_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define FACE_ID 2
#define POS_ID 1

class Face
{
public:

    /** 3 dimensions to identify each face. Each face has its own unique ID.
      * The vector is in three dimensions and is composed of integers, two of which
      * are +-POS_ID, and one of which is +-FACE_ID. For example, the the face on the
      * top left front corner, the face on the left side, would have an ID of
      * (FACE_ID, POS_ID, -POS_ID). Where the magnitude of each direction is
      * Left+ Right-, Up+ Down-, Back+ Front- */
    glm::ivec3 posID;

    Face() {}

    Face(glm::ivec3 posID, glm::vec3 *colour, float cubeSideLength)
        : colour(colour)
        , posID(posID)
    {
        initModelMatrix(cubeSideLength);
        
        // Initialize VAO, VBO, EBO, and borderEBO
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &borderEBO);

        glBindVertexArray(VAO);

        // Upload vertex data
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices[0]), vertices.data(), GL_STATIC_DRAW);

        // Upload filled plane indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

        // Define vertex attributes
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Upload border indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, borderIndices.size() * sizeof(borderIndices[0]), borderIndices.data(), GL_STATIC_DRAW);

        // Unbind VAO and buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~Face()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void render2(glm::mat4 projectionView, GLuint shaderID)
    {
        // Rotate and create matrix
        if (rotating) rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotationAxis);
        glm::mat4 mvp = projectionView * rotation * model;

        // Set shader uniforms
        GLuint location;
        glUseProgram(shaderID);
        location = glGetUniformLocation(shaderID, "mvp");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));
        location = glGetUniformLocation(shaderID, "colour");
        glUniform3f(location, colour->x, colour->y, colour->z);
        
        // Render geometry
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }

    void render(glm::mat4 projectionView, GLuint shaderID)
    {
        // Rotate and create matrix
        if (rotating) 
            rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), rotationAxis);
        glm::mat4 mvp = projectionView * rotation * model;

        GLuint location;

        // **1. Render the border (outer edges only)**
        glUseProgram(shaderID);
        location = glGetUniformLocation(shaderID, "mvp");
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mvp));
        location = glGetUniformLocation(shaderID, "colour");
        glUniform3f(location, 0.0f, 0.0f, 0.0f); // Set border color to black

        glBindVertexArray(VAO);

        // Bind the border indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, borderEBO);
        glLineWidth(borderSize);
        glDrawElements(GL_LINES, borderIndices.size(), GL_UNSIGNED_INT, 0);

        // **2. Render the filled plane**
        location = glGetUniformLocation(shaderID, "colour");
        glUniform3f(location, colour->x, colour->y, colour->z); // Set to desired fill color

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Bind the original EBO
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }




    void perFrame(float dt)
    {
        if (rotating)
        {
            float timeNormalized = rotationElapsedTime / rotationDuration;
            angle = angleFinal*bezier(timeNormalized);
            rotationElapsedTime += dt;

            if (rotationElapsedTime > rotationDuration)
            {
                rotating = false;
                rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleFinal), rotationAxis);
                model = rotation * model;
                rotation = glm::mat4(1.0f);
            }
        }
    }

    void beginRotation(glm::vec3 newRotationAxis)
    {
        // Finish the animation immediately before starting a new one
        if (rotating)
        {
            rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleFinal), rotationAxis);
            model = rotation * model;
            rotation = glm::mat4(1.0f);
        }
        
        // Start the animation
        rotating = true;
        rotationElapsedTime = 0.0f;
        angle = 0.0f;
        rotationAxis = newRotationAxis;

        // Update the position ID
        if      (newRotationAxis.x == -1.0f) rotatePosID('x', false);
        else if (newRotationAxis.x ==  1.0f) rotatePosID('x', true);
        else if (newRotationAxis.y == -1.0f) rotatePosID('y', false);
        else if (newRotationAxis.y ==  1.0f) rotatePosID('y', true);
        else if (newRotationAxis.z == -1.0f) rotatePosID('z', false);
        else if (newRotationAxis.z ==  1.0f) rotatePosID('z', true);
    }

private:

    // Rotation
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::vec3 rotationAxis;
    float angle = 0.0f;
    float angleFinal = 90.0f;
    float rotationDuration= 0.15f;
    float rotationElapsedTime = 0.0f;
    bool rotating = false;
    
    // Appearance
    float pieceScale = 0.85f;
    float borderSize = 2.0f;
    glm::vec3 *colour;
    glm::mat4 model = glm::mat4(1.0f);
    
    // Rotation bezier parameters
    glm::vec2 P1 = glm::vec2(0.42f, 0.00f);
    glm::vec2 P2 = glm::vec2(1.00f, 1.00f);

    // Vertices
    GLuint VAO, VBO, EBO, borderEBO;
    std::vector<float> vertices = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 0.0f, -0.5f, 0.5f, 0.0f };
    std::vector<unsigned int> indices = { 0, 1, 2, 2, 3, 0 };
    std::vector<unsigned int> borderIndices = { 0, 1, 1, 2, 2, 3, 3, 0 };

    void initModelMatrix(float cubeSideLength)
    {
        // Compute initial rotation based on the posID
        float initialAngle = 90.0f; glm::vec3 axis(0.0f, 0.0f, 1.0f);
        if      (glm::abs(posID.x) == FACE_ID) axis = glm::vec3(0.0f, 1.0f, 0.0f);
        else if (glm::abs(posID.y) == FACE_ID) axis = glm::vec3(1.0f, 0.0f, 0.0f);
        else if (glm::abs(posID.z) == FACE_ID) initialAngle = 0.0f;
        else fprintf(stderr, "Invalid initial face identification value\n");

        // Compute initial world position
        float faceUnit = cubeSideLength / 3.0f;
        glm::vec3 faceSigns = glm::sign(posID);
        glm::vec3 worldPos = faceUnit * faceSigns;

        // Extrude the face from the origin of their piece based on their face ID
        if      (glm::abs(posID.x) == FACE_ID) worldPos.x += glm::sign(posID.x) * faceUnit / 2.0f;
        else if (glm::abs(posID.y) == FACE_ID) worldPos.y += glm::sign(posID.y) * faceUnit / 2.0f;
        else if (glm::abs(posID.z) == FACE_ID) worldPos.z += glm::sign(posID.z) * faceUnit / 2.0f;

        // Build initial model matrix
        model = glm::translate(model, worldPos);
        // model = glm::translate(model, glm::vec3((float)posID.x, (float)posID.y, (float)posID.z) / 2.0f);  // TODO: Translate from position ID to world position
        model = glm::rotate(model, glm::radians(initialAngle), axis);
        model = glm::scale(model, glm::vec3(cubeSideLength * pieceScale / 3.0f));
    }

    float bezier(float t)
    {
        float u = 1 - t;
        float tt = t*t;
        float uut = u*u*t;
        float utt = u*tt;
        float ttt = t*tt;
        glm::vec2 p = 3*uut*P1 + 3*utt*P2 + glm::vec2(ttt);
        return p.y;
    }

    void rotatePosID(char axis, bool clockwise = true)
    {
        const int cw = clockwise ? 1 : -1;
        int x = posID.x, y = posID.y, z = posID.z;

        if      (axis == 'x') posID = glm::ivec3(x, cw * -z, cw * y);
        else if (axis == 'y') posID = glm::ivec3(cw * z, y, cw * -x);
        else if (axis == 'z') posID = glm::ivec3(cw * -y, cw * x, z);
    }

};

#endif