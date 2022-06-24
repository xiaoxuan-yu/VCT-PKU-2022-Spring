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

bool firstMouse = true;
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

bool mouseCursor = false;
bool controlPressed = false;
Camera camera(glm::vec3(0.0f, 4.0f, 0.0f));

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWInput input);

int main()
{
    //initialize window, renderer, camera, light and input_controller
    GLFWWindow m_window(SCR_WIDTH, SCR_HEIGHT, "Voxel Cone Tracing", fullscreen);
    VCTRenderer renderer(&m_window);
    GLFWInput InputController;
    InputController.bindInputtoWindow(m_window);
    renderer.init(&camera);

    while (!glfwWindowShouldClose(m_window.getGLFWwindow())) {

        glfwPollEvents();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(InputController);
        renderer.update(deltaTime);

        m_window.swapBuffers();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
    //
    return 0;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    if (mouseCursor)
        camera.ProcessMouseMovement(xoffset, yoffset);
}


void processInput(GLFWInput input)
{
    if (input.isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(input.getBoundWindow(), true);

    if (input.isKeyPressed(GLFW_KEY_W))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (input.isKeyPressed(GLFW_KEY_S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (input.isKeyPressed(GLFW_KEY_A))
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (input.isKeyPressed(GLFW_KEY_D))
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (input.isKeyPressed(GLFW_KEY_E))
        camera.ProcessKeyboard(UP, deltaTime);
    if (input.isKeyPressed(GLFW_KEY_Q))
        camera.ProcessKeyboard(DOWN, deltaTime);
    //空格键是否被按下，用来切换输入模式
    if ((input.isKeyPressed(GLFW_KEY_LEFT_CONTROL) || input.isKeyPressed(GLFW_KEY_RIGHT_CONTROL)))
    {
        //controlPressed = true;
        mouseCursor = !mouseCursor;
    }
    glfwSetCursorPosCallback(input.getBoundWindow(), mouse_callback);
    glfwSetInputMode(input.getBoundWindow(), GLFW_CURSOR, mouseCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
