#include "Devices.h"
#include <GLFW/glfw3.h>
#include <cstdio>


void Devices::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));// normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}


void Devices::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;
    ProcessMouseScroll(static_cast<float>(yoffset));
}

void Devices::MouseCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    (void)window;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;// reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    ProcessMouseMovement(xoffset, yoffset);
}


void Devices::UpdateCurrentFrame()
{
    float currentFrame = static_cast<float>(glfwGetTime());
    m_deltaTime = currentFrame - m_lastFrame;
    m_lastFrame = currentFrame;
}


void Devices::processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        this->ProcessKeyboard(Camera_Movement::FORWARD, m_deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        this->ProcessKeyboard(Camera_Movement::BACKWARD, m_deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        this->ProcessKeyboard(Camera_Movement::LEFT, m_deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        this->ProcessKeyboard(Camera_Movement::RIGHT, m_deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        this->ProcessKeyboard(Camera_Movement::Up, m_deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        this->ProcessKeyboard(Camera_Movement::Down, m_deltaTime);
    else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        this->m_mouseStatus = GLFW_CURSOR_DISABLED;
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        this->m_mouseStatus = GLFW_CURSOR_NORMAL;
    }
    return;
}

void Devices::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Devices::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    if (std::abs(xoffset) >= 1024 || std::abs(yoffset) >= 1024)
        return;

    if (m_mouseStatus == GLFW_CURSOR_NORMAL)
        return;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}


void Devices::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    if (direction == Camera_Movement::FORWARD)
        Position += Front * velocity;
    else if (direction == Camera_Movement::BACKWARD)
        Position -= Front * velocity;
    else if (direction == Camera_Movement::LEFT)
        Position -= Right * velocity;
    else if (direction == Camera_Movement::RIGHT)
        Position += Right * velocity;
    else if (direction == Camera_Movement::Up)
        Position.y += 1 * velocity;
    else if (direction == Camera_Movement::Down)
        Position.y -= 1 * velocity;
}


const glm::mat4 Devices::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}


Devices::Devices(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    firstMouse = true;
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    MouseSensitivity = SENSITIVITY;
    updateCameraVectors();
}

Devices::Devices(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    m_mouseStatus = 0;
    firstMouse = true;
    updateCameraVectors();
}
