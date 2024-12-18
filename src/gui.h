#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "cube.h"
#include "camera.h"

class GUI
{
public:

    GUI() {}

    GUI(Cube *cube, Camera *camera)
        : cube(cube)
        , camera(camera)
    {}

    void show()
    {
        cameraSettings();
    }

private:

    Cube *cube;
    Camera *camera;

    void cameraSettings()
    {
        ImGui::Begin("Camera Settings");

        static glm::vec3 newPosition = camera->position;
        static glm::vec3 newTarget = camera->target;
        static glm::vec3 newUpVector = camera->upVector;
        static float fovyDegrees = camera->fovy;
        static float newZFar = camera->zFar;
        static float newZNear = camera->zNear;

        if (ImGui::DragFloat("FOV", &fovyDegrees, 0.1f, -360.0f, 360.0f))
            camera->setFovy(fovyDegrees);
        if (ImGui::DragFloat("Z-Near", &newZNear, 0.1f))
            camera->setZNear(newZNear);
        if (ImGui::DragFloat("Z-Far", &newZFar, 0.1f))
            camera->setZFar(newZFar);
        if (ImGui::DragFloat3("Position", &newPosition[0]), 0.1f)
            camera->setPosition(newPosition);
        if (ImGui::DragFloat3("Target", &newTarget[0]), 0.1f)
            camera->setTarget(newTarget);
        if (ImGui::DragFloat3("Up Vector", &newUpVector[0]), 0.1f)
            camera->setUpVector(newUpVector);

        ImGui::End();
    }

};

#endif