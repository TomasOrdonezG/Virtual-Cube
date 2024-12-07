#ifndef CUBE_H
#define CUBE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "face.h"
#include "shader.h"

#define NUM_FACES 54
#define NUM_MOVES 12
#define PRINT_VEC4(v) (printf("(%.2f, %.2f, %.2f, %.2f)\n", (v).x, (v).y, (v).z, (v).w))

class Cube
{
private:  // Vertex and fragments shaders

    const char *vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPos;
        uniform mat4 mvp;

        void main() {
            gl_Position = mvp * vec4(aPos, 1.0);
        }
    )";

    const char *fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec3 colour;

        void main() {
            FragColor = vec4(colour.xyz, 1.0);
        }
    )";

public:

    float sideLength = 1.0f;

    Face *faces[NUM_FACES];

    Cube() : shader(vertexShaderSource, fragmentShaderSource)
    {
        srand(time(NULL));
        
        // Generate faces
        int i = 0;
        for (int f = -1; f <= 1; f += 2)
        {
            for (int a = -1; a <= 1; a++)
            {
                for (int b = -1; b <= 1; b++)
                {
                    int faceID = FACE_ID*f;
                    int ap = POS_ID*a;
                    int bp = POS_ID*b;
                    faces[i] = createFace(glm::ivec3(faceID, ap, bp));
                    faces[i+1] = createFace(glm::ivec3(ap, faceID, bp));
                    faces[i+2] = createFace(glm::ivec3(ap, bp, faceID));
                    i += 3;
                }
            }
        }
    }

    ~Cube()
    {
        for (int i = 0; i < NUM_FACES; i++)
        {
            delete faces[i];
        }
    }

    void render(glm::mat4 projectionView)
    {
        for (int i = 0; i < NUM_FACES; i++)
        {
            faces[i]->render(projectionView, shader.ID);
        }
    }

    void perFrame(float dt)
    {
        for (int i = 0; i < NUM_FACES; i++)
        {
            faces[i]->perFrame(dt);
        }
    }

    void move(const char *move)
    {
        #define SINGLE_MOVE(STR, COORD, CMP, AXIS) do {          \
            if (strcmp(move, STR) == 0)                     \
                for (int i = 0; i < NUM_FACES; i++)         \
                    if (faces[i]->posID.COORD CMP POS_ID)   \
                        faces[i]->beginRotation(AXIS);      \
        } while (0)
        SINGLE_MOVE("U" , y, >=  , glm::vec3( 0.0f, -1.0f,  0.0f));
        SINGLE_MOVE("U'", y, >=  , glm::vec3( 0.0f,  1.0f,  0.0f));
        SINGLE_MOVE("D" , y, <= -, glm::vec3( 0.0f,  1.0f,  0.0f));
        SINGLE_MOVE("D'", y, <= -, glm::vec3( 0.0f, -1.0f,  0.0f));
        SINGLE_MOVE("R" , x, <= -, glm::vec3( 1.0f,  0.0f,  0.0f));
        SINGLE_MOVE("R'", x, <= -, glm::vec3(-1.0f,  0.0f,  0.0f));
        SINGLE_MOVE("L" , x, >=  , glm::vec3( 1.0f,  0.0f,  0.0f));
        SINGLE_MOVE("L'", x, >=  , glm::vec3(-1.0f,  0.0f,  0.0f));
        SINGLE_MOVE("F" , z, <= -, glm::vec3( 0.0f,  0.0f,  1.0f));
        SINGLE_MOVE("F'", z, <= -, glm::vec3( 0.0f,  0.0f, -1.0f));
        SINGLE_MOVE("B" , z, >=  , glm::vec3( 0.0f,  0.0f, -1.0f));
        SINGLE_MOVE("B'", z, >=  , glm::vec3( 0.0f,  0.0f,  1.0f));
    }

    void keyCallback(int key)
    {
        // Scramble
        if (key == GLFW_KEY_SPACE) scramble();
        
        // Single moves
        #define EXECUTE_MOVE(KEY, MOVE) do {  \
            if (key == GLFW_KEY_ ## KEY)      \
                move(MOVE);                   \
        } while (0)
        EXECUTE_MOVE(J, "U" );
        EXECUTE_MOVE(F, "U'");
        EXECUTE_MOVE(S, "D" );
        EXECUTE_MOVE(L, "D'");
        EXECUTE_MOVE(I, "R" );
        EXECUTE_MOVE(K, "R'");
        EXECUTE_MOVE(E, "L" );
        EXECUTE_MOVE(D, "L'");
        EXECUTE_MOVE(H, "F" );
        EXECUTE_MOVE(G, "F'");
        EXECUTE_MOVE(W, "B" );
        EXECUTE_MOVE(O, "B'");

        // Cube rotations
        #define EXECUTE_ROTATION(KEY, COORD, AXIS) do {    \
            if (key == GLFW_KEY_ ## KEY)            \
                for (int i = 0; i < NUM_FACES; i++) \
                    faces[i]->beginRotation(AXIS);  \
        } while (0)
        EXECUTE_ROTATION(        T, x, glm::vec3( 1.0f,  0.0f,  0.0f));  // T: x
        EXECUTE_ROTATION(        Y, x, glm::vec3( 1.0f,  0.0f,  0.0f));  // Y: x
        EXECUTE_ROTATION(        V, x, glm::vec3(-1.0f,  0.0f,  0.0f));  // B: x'
        EXECUTE_ROTATION(        B, x, glm::vec3(-1.0f,  0.0f,  0.0f));  // N: x'
        EXECUTE_ROTATION(SEMICOLON, y, glm::vec3( 0.0f, -1.0f,  0.0f));  // ;: y
        EXECUTE_ROTATION(        A, y, glm::vec3( 0.0f,  1.0f,  0.0f));  // A: y'
        EXECUTE_ROTATION(        P, z, glm::vec3( 0.0f,  0.0f,  1.0f));  // P: z
        EXECUTE_ROTATION(        Q, z, glm::vec3( 0.0f,  0.0f, -1.0f));  // Q: z'

        // TODO: Wide moves

        // TODO: Slice moves
    }

    void scramble()
    {
        int moveCount = 20;
        for (int i = 0; i < moveCount; i++)
        {
            int randIndex = rand() % NUM_MOVES;
            move(moves[randIndex]);
        }
    }

private:

    Shader shader;

    const char *moves[12] = { "U", "U'", "D", "D'", "R", "R'", "L", "L'", "F", "F'", "B", "B'" };

    struct {
        glm::vec3 orange = glm::vec3(1.0f, 0.5f, 0.0f);
        glm::vec3    red = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 yellow = glm::vec3(1.0f, 1.0f, 0.0f);
        glm::vec3  white = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3   blue = glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3  green = glm::vec3(0.0f, 1.0f, 0.0f);
    } colours;

    Face *createFace(glm::ivec3 initialPos)
    {
        glm::vec3 *colour;
        if      (initialPos.x == -FACE_ID) colour = &colours.orange;
        else if (initialPos.x ==  FACE_ID) colour = &colours.red;
        else if (initialPos.y == -FACE_ID) colour = &colours.white;
        else if (initialPos.y ==  FACE_ID) colour = &colours.yellow;
        else if (initialPos.z == -FACE_ID) colour = &colours.green;
        else if (initialPos.z ==  FACE_ID) colour = &colours.blue;
        return new Face(initialPos, colour, sideLength);
    }

};

#endif