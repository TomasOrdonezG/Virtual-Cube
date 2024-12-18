#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "window.h"

class Camera
{
public:

    int cachedWindowWidth, cachedWindowHeight;
    glm::vec3 position, target, upVector;
    glm::mat4 projection, view, projectionView;
    float fovy, zNear, zFar;


    Camera() {}

    Camera(glm::vec3 cameraPos, glm::vec3 cameraTarget, int windowWidth, int windowHeight)
        : position(cameraPos)
        , target(cameraTarget)
        , upVector(0.0f, 1.0f, 0.0f)
        , fovy(glm::radians(45.0f))
        , zNear(0.1f)
        , zFar(100.0f)
        , projection(glm::perspective(glm::radians(45.0f), (float)(windowWidth) / (float)(windowHeight), 0.1f, 100.0f))
        , view(glm::lookAt(cameraPos, cameraTarget, upVector))
        , projectionView(projection*view)
        , cachedWindowWidth(windowWidth)
        , cachedWindowHeight(windowHeight)
    {}

    void onWindowResize(int newWindowWidth, int newWindowHeight)
    {
        cachedWindowWidth = newWindowWidth;
        cachedWindowHeight = newWindowHeight;
        updateProjection();
    }

    void setPosition(glm::vec3 newCameraPos)
    {
        position = newCameraPos;
        updateView();
    }

    void setTarget(glm::vec3 newCameraTarget)
    {
        target = newCameraTarget;
        updateView();
    }

    void setUpVector(glm::vec3 newUpVector)
    {
        upVector = newUpVector;
        updateView();
    }

    void setFovy(float degrees)
    {
        fovy = glm::radians(degrees);
        updateProjection();
    }

    void setZNear(float newZNear)
    {
        zNear = newZNear;
        updateProjection();
    }

    void setZFar(float newZFar)
    {
        zFar = newZFar;
        updateProjection();
    }

private:

    void updateView()
    {
        view = glm::lookAt(position, target, upVector);
        projectionView = projection*view;
    }

    void updateProjection()
    {
        projection = glm::perspective(fovy, (float)(cachedWindowWidth) / (float)(cachedWindowHeight), zNear, zFar);
        projectionView = projection*view;
    }

};

#endif