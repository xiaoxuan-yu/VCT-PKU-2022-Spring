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

bool mouseCursor = true;
Camera camera(glm::vec3(0.0f, 4.0f, 0.0f));
Light light;

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWInput *input);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main()
{
    //initialize window, renderer, camera, light and input_controller
    GLFWWindow m_window(SCR_WIDTH, SCR_HEIGHT, "Voxel Cone Tracing", fullscreen);
    GLFWInput InputController;
    VCTRenderer renderer(&m_window, &InputController);
    InputController.bindInputtoWindow(m_window);
    renderer.init(&camera, &light);
    glfwSetFramebufferSizeCallback(m_window.getGLFWwindow(), framebuffer_size_callback);
    glfwSetScrollCallback(m_window.getGLFWwindow(), scroll_callback);

    while (!glfwWindowShouldClose(m_window.getGLFWwindow())) {

        glfwPollEvents();
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwGetWindowSize(m_window.getGLFWwindow(), &SCR_WIDTH, &SCR_HEIGHT);
        m_window.updateWindowSize(SCR_WIDTH, SCR_HEIGHT);
        renderer.updateWindowSize(SCR_WIDTH, SCR_HEIGHT);
        processInput(&InputController);

        renderer.render(deltaTime);
        //renderer.voxel_visualize(deltaTime);
        //renderer.depth_visualize(deltaTime);
        

        m_window.swapBuffers();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
    // return 0;
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


void processInput(GLFWInput *input)
{
    if (input->isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(input->getBoundWindow(), true);

    if (input->isKeyPressed(GLFW_KEY_W))
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (input->isKeyPressed(GLFW_KEY_S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (input->isKeyPressed(GLFW_KEY_A))
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (input->isKeyPressed(GLFW_KEY_D))
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (input->isKeyPressed(GLFW_KEY_E))
        camera.ProcessKeyboard(UP, deltaTime);
    if (input->isKeyPressed(GLFW_KEY_Q))
        camera.ProcessKeyboard(DOWN, deltaTime);
    //�ո���Ƿ񱻰��£������л�����ģʽ
    if ((input->isKeyPressedOnce(GLFW_KEY_LEFT_CONTROL) || input->isKeyPressedOnce(GLFW_KEY_RIGHT_CONTROL)))
    {
        //controlPressed = true;
        mouseCursor = !mouseCursor;
    }
    //renderer
    if (input->isKeyPressedOnce(GLFW_KEY_Z))
        input->isConeTracing = !(input->isConeTracing);
    if (input->isKeyPressedOnce(GLFW_KEY_X))
        input->diffuse = !(input->diffuse);
    if (input->isKeyPressedOnce(GLFW_KEY_C))
        input->specular = !(input->specular);
    if (input->isKeyPressedOnce(GLFW_KEY_V))
        input->ambient = !(input->ambient);

    glfwSetCursorPosCallback(input->getBoundWindow(), mouse_callback);
    glfwSetInputMode(input->getBoundWindow(), GLFW_CURSOR, mouseCursor ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
