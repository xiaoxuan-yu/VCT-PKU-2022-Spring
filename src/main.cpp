#include <iostream>
#include "VCTRenderer.h"
#include "Timer.h"
#include "Window.h"

int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;
bool fullscreen = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main()
{
    //initialize window, render, camera and light
    GLFWWindow m_window(SCR_WIDTH, SCR_HEIGHT, "Voxel Cone Tracing", fullscreen);
    VCTRenderer renderer(&m_window);
    Camera camera(glm::vec3(0.0f, 4.0f, 0.0f));
    renderer.init(&camera);

    while (!glfwWindowShouldClose(m_window.getGLFWwindow())) {

        glfwPollEvents();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //processInput(window);
        renderer.update(deltaTime);

        m_window.swapBuffers();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
    //
    return 0;
}