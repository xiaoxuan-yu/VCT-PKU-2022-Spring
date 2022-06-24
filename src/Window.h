#pragma once

#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GLFWWindow
{
public:
    GLFWwindow* m_window = nullptr;

    GLFWWindow(int width, int height, const std::string& windowName, bool fullscreen) :
        m_width(width),
        m_height(height),
        m_windowName(windowName),
        m_fullscreen(fullscreen)
    {
        initialize();
    }

    ~GLFWWindow()
    {
        glfwTerminate();
    }

    int getWidth() { return m_width; }

    int getHeight() { return m_height; }

    GLFWwindow* getGLFWwindow() const
    {
        return m_window;
    }

    void swapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    void setWindowTitle(const char* title)
    {
        glfwSetWindowTitle(m_window, title);
    }

    void setDefaultWindowTitle()
    {
        glfwSetWindowTitle(m_window, m_windowName.c_str());
    }

    void setCursorVisibility(float m_showCursor) {
        if (!m_showCursor) {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        }
        else {
            glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }

    void updateWindowSize(int width, int height) {
        m_width = width;
        m_height = height;
    }

private:

    void initialize()
    {
        if (glfwInit() != GL_TRUE) {
            throw std::runtime_error("Could not initialize GLFW!");
        }

        glfwDefaultWindowHints();
        //glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

        m_window = glfwCreateWindow(m_width, m_height, m_windowName.c_str(), m_fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

        if (!m_window) {
            glfwTerminate();
            throw std::runtime_error("Could not open GLFW Window!");
        }

        setWindowTitle(m_windowName.c_str());

        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwMakeContextCurrent(m_window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
    }

    int	m_width = 1280;
    int m_height = 720;
    std::string	m_windowName;
    bool m_fullscreen = false;
};