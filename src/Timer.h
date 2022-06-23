#pragma once

#include <glfw/glfw3.h>

class Timer
{
private:
    double m_timestamp = 0.;

public:
    // Receives time since glfwInit() call
    double timestamp() const
    {
        return glfwGetTime();
    }

    void setTimestamp()
    {
        m_timestamp = timestamp();
    }

    // Returns the time difference since the last setTimestamp() call.
    double timestampDiff() const
    {
        return glfwGetTime() - m_timestamp;
    }

};
