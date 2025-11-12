// 一个能自由移动的camera

#ifndef CAMERA_H
#define CAMERA_H

#include "GLDefine.h"
#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// Defines several possible options for camera movement. Used as abstraction to
// stay away from window-system specific input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, Up, Down };

// Default camera values
constexpr static const float YAW = -90.0f;
constexpr static const float PITCH = 0.0f;

constexpr static const float SPEED = 2.5f;
constexpr static const float SENSITIVITY = 0.1f;
constexpr static const float ZOOM = 45.0f;

using namespace BaseTypeLen;

// An abstract camera class that processes input and calculates the
// corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Devices {
public:
  // camera Attributes
  glm::vec3 Position;
  glm::vec3 Front;
  glm::vec3 Up;
  glm::vec3 Right;
  glm::vec3 WorldUp;
  // euler Angles
  float Yaw;
  float Pitch;
  // camera options
  float MovementSpeed;
  float MouseSensitivity;
  float Zoom;

  Devices() = delete;
  // constructor with vectors
  Devices(glm::vec3 position = vec3Zero, glm::vec3 up = bcCamUp,
          float yaw = YAW, float pitch = PITCH);
  // constructor with scalar values
  Devices(float posX, float posY, float posZ, float upX, float upY, float upZ,
          float yaw, float pitch);

  // returns the view matrix calculated using Euler Angles and the LookAt Matrix
  const glm::mat4 GetViewMatrix() const;

  // processes input received from any keyboard-like input system. Accepts input
  // parameter in the form of camera defined ENUM (to abstract it from windowing
  // systems)
  void ProcessKeyboard(Camera_Movement direction, float deltaTime);

  // processes input received from a mouse input system. Expects the offset
  // value in both the x and y direction.
  void ProcessMouseMovement(float xoffset, float yoffset,
                            GLboolean constrainPitch = true);
  void ProcessMouseScroll(float yoffset);
  void MouseCallback(GLFWwindow *window, double xposIn, double yposIn);
  void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
  // keyboard
  void processInput(GLFWwindow *window);
  void UpdateCurrentFrame();

private:
  void updateCameraVectors();

private:
  bool firstMouse;
  int lastX;
  int lastY;
  float m_deltaTime;
  float m_lastFrame;

  int m_mouseStatus;
};
#endif