//
// Created by Aquito on 2025/2/27.
//

#ifndef FUNCTION_H
#define FUNCTION_H

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "Devices.h"

#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"

#define ProcessExit(tips)                                                      \
  {                                                                            \
    std::cout << __FILE__ << ":" << __LINE__ << ": " << tips << std::endl;     \
    glfwTerminate();                                                           \
    exit(-1);                                                                  \
  }

namespace Function {

namespace OpenGLBase {

enum ScreenRatio { SR_1K = 0, SR_2k = 1, SR_3K = 2, SR_4K = 3 };

// OpenGL Version: 330;
// OpenGL Profile: Core
void SetWindowPos(GLFWwindow *win, int width, int height, ScreenRatio sr);
bool OpenGLInit(GLFWwindow **win, const int &width, const int &height,
                const char *title, ScreenRatio sr = ScreenRatio::SR_1K);

bool OpenGLInit(GLFWwindow **win, const int &width, const int &height,
                const std::pair<int, int> &version, const char *title,
                ScreenRatio sr = ScreenRatio::SR_1K);

void EnableMouse(GLFWwindow *win, const Devices &device);
void func_LoadImage(const char *imgPath, int &width, int &height,
                    int &nrChannels, unsigned char **data, bool flipY = true);
// 需要注意环绕和过滤方
GLuint BindTextureBuffer(const std::string &texPath, bool flipY = true);
GLuint BindTextureBuffer(const char *imgPath, bool flipY = true);
bool BindTextureBuffer(GLuint &texture, const char *imgPath, bool flipY = true);
GLuint BindTextureBuffer_Cubemap(const std::string &texPath, bool flipY = true);
GLuint BindTextureBuffer_Cubemap(const char *imgPath, bool flipY = true);
bool BindTextureBuffer_Cubemap(GLuint &texture, const char *imgPath,
                               bool flipY = true);

bool GetTexture(GLuint &texture, const char *imgPath);
bool GetTexture(GLuint &texture, const std::string &imgPath);

GLuint GetTexture(const char *imgPath);
GLuint GetTexture(const std::string &imgPath);

GLuint BindBoxTexture(const std::string &basePath,
                      const std::vector<std::string> &picFileName);
GLuint BindBoxTexture(const std::vector<std::string> &picPaths);

inline glm::mat4 perspective(const Devices &device, float aspect) {
  return glm::perspective(glm::radians(device.Zoom), aspect, 0.1f, 100.0f);
}

inline glm::mat4 perspective(const float &angle, float aspect) {
  return glm::perspective(glm::radians(angle), aspect, 0.1f, 100.0f);
}

void ShowGraphCardInfo();
} // namespace OpenGLBase

// 对部分OpenGL函数的实现
typedef std::vector<std::vector<float>> mat;
namespace OpenGLMath {

void SetBool(const GLuint &sp, const std::string uniformStr, const bool &val);

void SetInt(const GLuint &sp, const std::string uniformStr, const int &val);
void SetInt(const GLuint &sp, const char *uniformStr, const int &val);

void SetVec2(const GLuint &sp, const std::string uniformStr,
             const glm::vec2 &val);
void SetVec2(const GLuint &sp, const char *uniformStr, const glm::vec2 &val);

void SetVec3(const GLuint &sp, const std::string uniformStr,
             const glm::vec3 &val);
void SetVec3(const GLuint &sp, const char *uniformStr, const glm::vec3 &val);

void SetVec4(const GLuint &sp, const std::string uniformStr,
             const glm::vec4 &val);
void SetVec4(const GLuint &sp, const char *uniformStr, const glm::vec4 &val);

void SetFloat(const GLuint &sp, const std::string uniformStr, const float &val);
void SetFloat(const GLuint &sp, const char *uniformStr, const float &val);

void SetMat4(const GLuint &sp, const std::string uniformStr,
             const glm::mat4 &val);
void SetMat4(const GLuint &sp, const char *uniformStr, const glm::mat4 &val);

Function::mat LookAt(const std::vector<float> &position,
                     const std::vector<float> &target,
                     const std::vector<float> &worldUp);
} // namespace OpenGLMath

} // namespace Function

#endif // FUNCTION_H
