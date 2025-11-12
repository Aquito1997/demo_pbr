#include "glad/glad.h"

#include "GLFW/glfw3.h"
#include "glm/ext/matrix_float4x4.hpp"
#include <GLDefine.h>

#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace Function
{
namespace StencilTest
{
static const std::map<const std::string, int> stencilMap = {
    {"GL_KEEP", GL_KEEP},
    {"GL_ZERO", GL_ZERO},
    {"GL_REPLACE", GL_REPLACE},
    {"GL_INCR", GL_INCR},
    {"GL_INCR_WRAP", GL_INCR_WRAP},
    {"GL_DECR", GL_DECR},
    {"GL_DECR_WRAP", GL_DECR_WRAP},
    {"GL_INVERT", GL_INVERT},
    {"GL_NEVER", GL_NEVER},
    {"GL_LESS", GL_LESS},
    {"GL_LEQUAL", GL_LEQUAL},
    {"GL_GREATER", GL_GREATER},
    {"GL_GEQUAL", GL_GEQUAL},
    {"GL_EQUAL", GL_EQUAL},
    {"GL_NOTEQUAL", GL_NOTEQUAL},
    {"GL_ALWAYS", GL_ALWAYS}};

// 将指定字符串转为对应的模板测试枚举值
int GetStencilTestEnum(const std::string& str);
};// namespace StencilTest

void FrameBufferSizeCallBack(GLFWwindow* window, int width, int height);
void EventExit(GLFWwindow* window);

[[maybe_unused]] GLFWmonitor* GetMonitor();

unsigned int CreateShader(const char* src, const unsigned int& shaderType);

unsigned int CreateShaderProgram(const char* vertexShaderSrc,
                                 const char* fragShaderSrc);
unsigned int CreateShaderProgram(const char* vertexShaderSrc,
                                 const char* geoShaderSrc,
                                 const char* fragShaderSrc);

void BindVertexFragment(unsigned int& vao, unsigned int& vbo, float* vertices,
                        int verNum);

// 在VAO中有颜色数据
[[maybe_unused]] void BindVertexFragmentWithColor(unsigned int& vao,
                                                  unsigned int& vbo,
                                                  float* vertices, int verNum);
void BindElement(unsigned int& vao, unsigned int& vbo, unsigned int& ebo,
                 float* vertices, int verNum, unsigned int* indices,
                 int indicNum);

std::string LoadShaderFile(const std::string& fp);

unsigned int LoadShaderProgram(const std::string& basePath,
                               const std::string& strVertPath,
                               const std::string& strFragPath);
unsigned int LoadShaderProgram(const std::string& strVertPath,
                               const std::string& strFragPath);
unsigned int LoadShaderProgram(const std::vector<std::string> glsl);
unsigned int BindShaderProgram(const std::string& vtexPath,
                               const std::string& GeoPath,
                               const std::string& FragPath);

/**
 * @param vtexPath: vertex file Path
 * @param GeoPath: Geometry file Path
 * @param FragPath: Fragment file Path
 */
unsigned int BuildShaderProgram(const std::string& vtexPath,
                                const std::string& GeoPath,
                                const std::string& FragPath);

namespace LogSimple
{
#define GL_TRACE(str)                                                         \
    do {                                                                      \
        std::cout << __FILE__ << ":" << __LINE__ << "  " << str << std::endl; \
    } while (0);                                                              \
    // namespace LogSimple

#define GL_CHECK()                                                             \
    while (GLenum err = glGetError())                                          \
        std::cerr << __FILE__ << ":" << __LINE__ << " - OpenGL error: " << err \
                  << std::endl;
}// namespace LogSimple

namespace LinuxDebug
{
void PrintStackTrace();

};// namespace LinuxDebug

namespace GLDebug
{
void PrintMat4(const glm::mat4& mat4);
void PrintMat3(const glm::mat3& mat4);
void PrintMat2(const glm::mat2& mat4);
void PrintVec4(const glm::vec4& mat4);
void PrintVec3(const glm::vec3& mat4);
void PrintVec2(const glm::vec2& mat4);

inline void PrintVertices(GLfloat* quadVertices)
{
    if (quadVertices == nullptr)
        return;

    for (size_t xCnt = 0; xCnt < 84; xCnt++)
    {
        if (xCnt % 14 == 0 && xCnt != 0)
        {
            std::cout << std::endl;
        }

        std::cout << quadVertices[xCnt];
        std::cout << "\t";
    }
    std::cout << std::endl;
}

inline void PrintDepthFramebuffer(int width, int height, size_t runTimes = 1,
                                  bool detail = false,
                                  bool indexDetail = false)
{
    static size_t cnt = 0;
    if (cnt >= runTimes)
    {
        cnt++;
        return;
    }
    cnt++;
    int x = 0, y = 0;
    std::vector<unsigned char> depthPixels(width * height * 4);
    glReadPixels(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT,
                 depthPixels.data());
    std::cout << "Start" << std::endl;
    std::vector<int> vecIndex;

    if (detail)
    {
        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                int offset = (row * width + col) * 4;
                printf("(%d,%d): [%3d, %3d, %3d, %3d]\n", col, row,
                       depthPixels[offset],    // R
                       depthPixels[offset + 1],// G
                       depthPixels[offset + 2],// B
                       depthPixels[offset + 3] // A
                );
                vecIndex.push_back(offset);
            }
        }
    }
    else
    {
        for (int row = 0; row < height; ++row)
        {
            for (int col = 0; col < width; ++col)
            {
                int offset = (row * width + col) * 4;
                if (depthPixels[offset + 0] > 0 || depthPixels[offset + 1] > 0 || depthPixels[offset + 2] > 0 || depthPixels[offset + 3] > 0)
                {
                    vecIndex.push_back(offset);
                }
            }
        }
    }

    if (indexDetail)
    {
        for (const auto& val : vecIndex)
        {
            std::cout << val << "\t";
        }
    }
    std::cout << "The valid number is :" << vecIndex.size() << std::endl;
    std::cout << "End" << std::endl;
}
///////////////////////////////////////////////////////////
}// namespace GLDebug
};// namespace Function
