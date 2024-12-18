#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "cube.h"
#include "window.h"
#include "gui.h"
#include "camera.h"

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 800

Window gameWindow;
Camera camera;
// Cube cube;
GUI cubeGUI;

void initImGui(GLFWwindow *window);

void windowResizeCallback(int newWidth, int newHeight)
{
    camera.onWindowResize(newWidth, newHeight);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    Cube *cube = static_cast<Cube*>(glfwGetWindowUserPointer(window));
    if (cube && action == GLFW_PRESS)
    {
        cube->keyCallback(key);
    }
}

void gameEvents()
{
    ImVec2 windowSize = ImGui::GetContentRegionAvail();
    
    // Resize Window, textures, etc
    static ImVec2 lastWindowSize;
    bool windowChangedSize = windowSize.x != lastWindowSize.x || windowSize.y != lastWindowSize.y;
    if (windowChangedSize)
    {
        gameWindow.updateDimensions((int)windowSize.x, (int)windowSize.y);
    }
    lastWindowSize = windowSize;
}

int main(int argc, char **argv)
{
    // Intialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Create GLFW window
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "float", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Error: Failed to create GLFW window\n");
        exit(1);
    }

    // GLFW Window Settings
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);

    glewInit();

    // OpenGL Settings
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initImGui(window);
    gameWindow = Window(WINDOW_WIDTH, WINDOW_HEIGHT, windowResizeCallback);


    // Camera
    glm::vec3 cameraPos(0.0f, 2.0f, -2.0f);
    glm::vec3 cameraTarget(0.0f, 0.0f, 0.0f);
    camera = Camera(cameraPos, cameraTarget, gameWindow.width, gameWindow.height);

    // Create cube and bind it to the window (so we can directly update the cube using keyboard interrupts)
    Cube cube(1.0f);
    glfwSetWindowUserPointer(window, &cube);

    // Create GUI instance
    cubeGUI = GUI(&cube, &camera);

    float t = 0.0f;
    glm::ivec2 lastFBOSize((int)gameWindow.width, (int)gameWindow.height);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        cubeGUI.show();

        // Calculate dt
        static float lastFrame = 0.0f;
        float currentFrame = glfwGetTime();
        float dt = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Get the size of the ImGui window
        ImGui::SetNextWindowDockID(ImGui::GetID("DockSpace"), ImGuiCond_FirstUseEver);
        ImGui::Begin("Cube");
        {
            gameEvents();

            glBindFramebuffer(GL_FRAMEBUFFER, gameWindow.FBO);
            glViewport(0, 0, gameWindow.width, gameWindow.height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Cube logic
            cube.perFrame(dt);
            cube.render(camera.projectionView);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Reset the viewport for the default framebuffer
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Display FBO texture to ImGui window
            ImGui::Image((ImTextureID)(intptr_t)gameWindow.texture, ImVec2(gameWindow.width, gameWindow.height), ImVec2(0, 1), ImVec2(1, 0));
        }
        ImGui::End();

        // Render
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Clean up
    glfwTerminate();

    return 0;
}

void initImGui(GLFWwindow *window)
{
    // Context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 

    // Style
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Renderer backend
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}
