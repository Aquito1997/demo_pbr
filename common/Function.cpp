//
// Created by Aquito on 2025/2/27.
//

#include "Function.h"
#include "FuncBase.h"

#include "glad/glad.h"

#include <GLFW/glfw3.h>

#include <cassert>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#ifdef __WIN32__
#include <libloaderapi.h>
#include <minwinbase.h>
#include <minwindef.h>
#elif defined(__linux__)
#include "StackTracer.hpp"
#include <alloca.h>
#include <execinfo.h>// for backtrace
#include <unistd.h>
#endif

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Devices.h"
#include "stb_image.h"

#define OpenGLErrChk Function::GLDebug::GetOpenGLErrorMsg(__FILE__, __LINE__)

// 找到一个分辨率大于 1080P 的宏
GLFWmonitor* Function::GetMonitor()
{
    int monitorCnt = 0;
    GLFWmonitor* retMonitor = nullptr;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCnt);
    if (monitors == nullptr)
        return nullptr;

    if (monitorCnt > 1)
    {
        int phyHeight = 0, phyWidth = 0;
        const char* cstrMonitorName = nullptr;
        for (int iCnt = 0; iCnt < monitorCnt; ++iCnt)
        {
            cstrMonitorName = glfwGetMonitorName(monitors[iCnt]);
            std::cout << cstrMonitorName << std::endl;
            cstrMonitorName = nullptr;

            glfwGetMonitorPhysicalSize(monitors[iCnt], &phyWidth, &phyHeight);
            if (phyWidth * phyHeight > 2000 * 1200)
            {
                retMonitor = monitors[iCnt];
                break;
            }
        }
        if (retMonitor == nullptr)
            retMonitor = monitors[0];
    }
    else
        retMonitor = monitors[0];
    return retMonitor;
}

unsigned int Function::CreateShader(const char* src,
                                    const unsigned int& shaderType)
{
    unsigned int vertexShader = glCreateShader(shaderType);
    glShaderSource(vertexShader, 1, &src, nullptr);
    glCompileShader(vertexShader);

    int success = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        int logSize = 0;
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
        char* infoLog = (char*)malloc(logSize * sizeof(char));
        memset(infoLog, 0, logSize);
        glGetShaderInfoLog(vertexShader, logSize, nullptr, infoLog);
        GL_TRACE(std::string("VerTex Shader 获取着色器的编译状态失败: ") + infoLog);
        free(infoLog);
    }
    return vertexShader;
}

unsigned int Function::CreateShaderProgram(const char* vertexShaderSrc,
                                           const char* fragShaderSrc)
{
    unsigned int vertexShader = CreateShader(vertexShaderSrc, GL_VERTEX_SHADER);
    unsigned int fragmentShader = CreateShader(fragShaderSrc, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = -1;
    {// 创建着色器程序
        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        int success = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            int logSize = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
            char* infoLog = (char*)malloc(logSize * sizeof(char));
            memset(infoLog, 0, logSize);
            glGetShaderInfoLog(vertexShader, logSize, nullptr, infoLog);
            GL_TRACE(std::string("Program Linking 链接着色器程序失败: ") + infoLog);
            free(infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }
    return shaderProgram;
}

unsigned int Function::CreateShaderProgram(const char* vertexShaderSrc,
                                           const char* geoShaderSrc,
                                           const char* fragShaderSrc)
{
    unsigned int vertexShader = CreateShader(vertexShaderSrc, GL_VERTEX_SHADER);
    unsigned int geometryShader = CreateShader(geoShaderSrc, GL_GEOMETRY_SHADER);
    unsigned int fragmentShader = CreateShader(fragShaderSrc, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = -1;
    {// 创建着色器程序
        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, geometryShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        int success = 0;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success)
        {
            int logSize = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);
            char* infoLog = (char*)malloc(logSize * sizeof(char));
            memset(infoLog, 0, logSize);
            glGetShaderInfoLog(vertexShader, logSize, nullptr, infoLog);
            GL_TRACE(std::string("Program Linking 链接着色器程序失败: ") + infoLog);
            free(infoLog);
        }

#if defined(__aarch64__) || defined(_M_ARM64)
        glProgramParameteriEXT(program, GL_GEOMETRY_VERTICES_OUT_EXT, 1024);
#endif

        glDeleteShader(vertexShader);
        glDeleteShader(geometryShader);
        glDeleteShader(fragmentShader);
    }
    return shaderProgram;
}

void Function::BindVertexFragment(unsigned int& vao, unsigned int& vbo,
                                  float* vertices, int verNum)
{
    // 绑定VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    glGenBuffers(1, &vbo);// 创建 buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verNum, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);// 设置顶点属性
    glBindVertexArray(0);        // 解绑array
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void Function::BindVertexFragmentWithColor(unsigned int& vao, unsigned int& vbo,
                                           float* vertices, int verNum)
{
    // 绑定VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    // 绑定VBO
    glGenBuffers(1, &vbo);// 创建 buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verNum, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);// 设置顶点属性
    glBindVertexArray(0);        // 解绑array
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
}

void Function::BindElement(unsigned int& vao, unsigned int& vbo,
                           unsigned int& ebo, float* vertices, int verNum,
                           unsigned int* indices, int indicNum)
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verNum, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicNum, indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);// 设置顶点属性
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Function::EventExit(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Function::FrameBufferSizeCallBack([[gnu::unused]] GLFWwindow* window,
                                       int width, int height)
{
    glViewport(0, 0, width, height);
}

std::string Function::LoadShaderFile(const std::string& fp)
{
    std::ifstream ifs(fp);
    if (!ifs.is_open())
    {
        std::cout << __FILE__ << ":" << __LINE__ << " Path invalid:  " << fp
                  << std::endl;
        abort();
    }
    std::stringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

unsigned int Function::LoadShaderProgram(const std::string& basePath,
                                         const std::string& strVertPath,
                                         const std::string& strFragPath)
{
    std::string vsPath, fsPath;
    std::string prgName = basePath;
    if (prgName.find('\\') != std::string::npos)// win
    {
        std::string vsStr, fsStr;
        prgName = prgName.substr(0, prgName.find_last_of('\\'));

        vsStr = std::regex_replace(strVertPath, std::regex("\\./"), "\\").c_str();
        fsStr = std::regex_replace(strFragPath, std::regex("\\./"), "\\").c_str();

        vsStr = std::regex_replace(vsStr, std::regex("/"), "\\").c_str();
        fsStr = std::regex_replace(fsStr, std::regex("/"), "\\").c_str();

        vsPath = prgName + std::string((vsStr));
        fsPath = prgName + std::string((fsStr));

        vsPath = std::regex_replace(vsPath, std::regex("\\\\"), "/").c_str();
        fsPath = std::regex_replace(fsPath, std::regex("\\\\"), "/").c_str();
    }
    else if (prgName.find('/') != std::string::npos)// linux
    {
        prgName = prgName.substr(0, prgName.find_last_of('/'));
        prgName = std::filesystem::absolute(("./")).string() + prgName;
        std::string vsStr =
            std::regex_replace(strVertPath, std::regex("\\./"), "/").c_str();
        std::string fsStr =
            std::regex_replace(strFragPath, std::regex("\\./"), "/").c_str();
        vsPath = prgName + std::string(vsStr);
        fsPath = prgName + std::string(fsStr);
    }

    std::cout << "vsPath: " << vsPath << std::endl;
    std::cout << "fsPath: " << fsPath << std::endl;

    std::string strVertSrc = Function::LoadShaderFile(vsPath);
    std::string strFragSrc = Function::LoadShaderFile(fsPath);
    return Function::CreateShaderProgram(strVertSrc.c_str(), strFragSrc.c_str());
}

unsigned int Function::LoadShaderProgram(const std::string& strVertPath,
                                         const std::string& strFragPath)
{
    std::string strFragSrc = Function::LoadShaderFile(strFragPath);
    std::string strVertSrc = Function::LoadShaderFile(strVertPath);
    return Function::CreateShaderProgram(strVertSrc.c_str(), strFragSrc.c_str());
}

unsigned int Function::LoadShaderProgram(const std::vector<std::string> glsl)
{
    if (glsl.size() != 3)
    {
        using namespace Function::LogSimple;
        GL_TRACE("Need 3 parameter");
        return 0;
    }

    std::string strVertSrc = Function::LoadShaderFile(glsl[0]);
    std::string strGeoSrc = Function::LoadShaderFile(glsl[1]);
    std::string strFragSrc = Function::LoadShaderFile(glsl[2]);
    return Function::CreateShaderProgram(strVertSrc.c_str(), strGeoSrc.c_str(),
                                         strFragSrc.c_str());
}

unsigned int Function::BindShaderProgram(const std::string& vsPath,
                                         const std::string& gsPath,
                                         const std::string& fsPath)
{
    std::string strVertSrc = Function::LoadShaderFile(vsPath);
    std::string strGeoSrc = Function::LoadShaderFile(gsPath);
    std::string strFragSrc = Function::LoadShaderFile(fsPath);
    return Function::CreateShaderProgram(strVertSrc.c_str(), strGeoSrc.c_str(),
                                         strFragSrc.c_str());
}

unsigned int Function::BuildShaderProgram(const std::string& vtexPath,
                                          const std::string& GeoPath,
                                          const std::string& FragPath)
{
    std::string strVertSrc = Function::LoadShaderFile(vtexPath);
    std::string strGeoSrc = Function::LoadShaderFile(GeoPath);
    std::string strFragSrc = Function::LoadShaderFile(FragPath);
    return Function::CreateShaderProgram(strVertSrc.c_str(), strGeoSrc.c_str(),
                                         strFragSrc.c_str());
}

void Function::OpenGLBase::SetWindowPos(GLFWwindow* win, int width, int height,
                                        ScreenRatio sr)
{

    int winX, winY;
    if (sr == ScreenRatio::SR_1K)
    {
        winX = abs((1920 - width) / 2);
        winY = abs((1080 - height) / 2);
        glfwSetWindowPos(win, winX, winY);
    }
    else if (sr == ScreenRatio::SR_2k)
    {
        winX = abs((2560 - width) / 2);
        winY = abs((1440 - height) / 2);
        glfwSetWindowPos(win, winX, winY);
    }
    else if (sr == ScreenRatio::SR_3K)
    {
        winX = abs((3440 - width) / 2);
        winY = abs((1440 - height) / 2);
        glfwSetWindowPos(win, winX, winY);
    }
    else if (sr == ScreenRatio::SR_4K)
    {
        winX = abs((3840 - width) / 2);
        winY = abs((2160 - height) / 2);
        glfwSetWindowPos(win, winX, winY);
    }
    else
    {
        glfwSetWindowPos(win, 500, 100);
    }
}

bool Function::OpenGLBase::OpenGLInit(GLFWwindow** win, const int& width,
                                      const int& height, const char* title,
                                      ScreenRatio sr)
{
    if (win == nullptr)
    {
        using namespace Function::LogSimple;
        GL_TRACE("nullptr ???");
        exit(-1);
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *win = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (*win == nullptr)
        ProcessExit("Failed to create GLFW window");

    SetWindowPos(*win, width, height, sr);

    glfwMakeContextCurrent(*win);
    if (*win == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(*win, Function::FrameBufferSizeCallBack);
    int retVal = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (retVal == 0)
        ProcessExit("Failed to initialize GLAD");
    glfwSwapInterval(0);// 关闭垂直同步
    return true;
}

bool Function::OpenGLBase::OpenGLInit(GLFWwindow** win, const int& width,
                                      const int& height,
                                      const std::pair<int, int>& version,
                                      const char* title, ScreenRatio sr)
{
    if (win == nullptr)
    {
        using namespace Function::LogSimple;
        GL_TRACE("nullptr ???");
        exit(-1);
    }
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, version.first);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, version.second);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *win = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (*win == nullptr)
        ProcessExit("Failed to create GLFW window");

    SetWindowPos(*win, width, height, sr);

    glfwMakeContextCurrent(*win);
    if (*win == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(*win, Function::FrameBufferSizeCallBack);
    int retVal = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    if (retVal == 0)
        ProcessExit("Failed to initialize GLAD");
    glfwSwapInterval(0);// 关闭垂直同步
    return true;
}

void Function::OpenGLBase::EnableMouse(GLFWwindow* win,
                                       const Devices& devices)
{
    glfwSetWindowUserPointer(win, (void*)&devices);
    auto mouseCallback = [](GLFWwindow* win, double xpos, double ypos) {
        Devices* xCam = static_cast<Devices*>(glfwGetWindowUserPointer(win));
        if (xCam)
            xCam->MouseCallback(win, xpos, ypos);
    };

    auto scrollCallback = [](GLFWwindow* win, double xpos, double ypos) {
        Devices* xCam = static_cast<Devices*>(glfwGetWindowUserPointer(win));
        if (xCam)
            xCam->ScrollCallback(win, xpos, ypos);
    };

    glfwSetCursorPosCallback(win, mouseCallback);
    glfwSetScrollCallback(win, scrollCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
}

void Function::OpenGLBase::func_LoadImage(const char* imgPath, int& width,
                                          int& height, int& nrChannels,
                                          unsigned char** data, bool flipY)
{
    if (data == nullptr)
    {
        using namespace Function::LogSimple;
        GL_TRACE(" Nullptr");
        abort();
    }
    // 反转图像的Y轴，达到和OpenGL一样的坐标系
    // static size_t cnt = 0;
    *data = stbi_load(imgPath, &width, &height, &nrChannels, 0);
    // stbi_write_png(("output.png" + std::to_string(++cnt)).c_str(), width,
    // height, 3, *data, width * nrChannels);
    if (flipY)// 垂直翻转纹理
    {
        unsigned char* flippedData = new unsigned char[width * height * nrChannels];
        for (int y = 0; y < height - 1; y++)
        {
#if defined(_WIN32) || defined(_WIN64)
            unsigned char* src = *data + (height - 1 - y) * width * nrChannels;
            unsigned char* dst = flippedData + y * width * nrChannels;
            CopyMemory(dst, src, width * nrChannels);
#elif defined(__linux__) || defined(__unix__)
            memcpy(flippedData + y * width * nrChannels,
                   *data + (height - y) * width * nrChannels, width * nrChannels);
#endif
        }
        stbi_image_free(*data);
        *data = flippedData;
    }
}

GLuint Function::OpenGLBase::BindTextureBuffer(const std::string& texPath,
                                               bool flipY)
{
    return BindTextureBuffer(texPath.c_str(), flipY);
}

GLuint Function::OpenGLBase::BindTextureBuffer(const char* imgPath,
                                               bool flipY)
{
    GLuint texture = 0;
    BindTextureBuffer(texture, imgPath, flipY);
    return texture;
}

bool Function::OpenGLBase::BindTextureBuffer(GLuint& texture,
                                             const char* imgPath, bool flipY)
{
    int width, height, nrChannels;
    unsigned char* imgData = nullptr;
    func_LoadImage(imgPath, width, height, nrChannels, &imgData, flipY);
    if (imgData == nullptr)
        return false;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 加载纹理
    if (nrChannels == 1)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED,
                     GL_UNSIGNED_BYTE, imgData);
    else if (nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, imgData);
    else if (nrChannels == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置环绕方式
    if (nrChannels == 3)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {// 设置为GL_CLAMP_TO_EDGE
        // 能解决在有透明度的图片中使用插值时能看到明显的边框
        // OpenGL会对边缘的值和纹理下一个重复的值进行插值
        // 因为我们将它的环绕方式设置为了GL_REPEAT。
        // 这通常是没问题的，但是由于我们使用了透明值，
        // 纹理图像的顶部将会与底部边缘的纯色值进行插值。
        // 这样的结果是一个半透明的有色边框，你可能会看见它环绕着你的纹理四边形。
        // 要想避免这个，每当你alpha纹理的时候，请将纹理的环绕方式设置为GL_CLAMP_TO_EDGE：
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    // 设置过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(imgData);
    return true;
}

GLuint
Function::OpenGLBase::BindTextureBuffer_Cubemap(const std::string& texPath,
                                                bool flipY)
{
    return BindTextureBuffer_Cubemap(texPath.c_str(), flipY);
}

GLuint Function::OpenGLBase::BindTextureBuffer_Cubemap(const char* imgPath,
                                                       bool flipY)
{
    GLuint texture = 0;
    BindTextureBuffer_Cubemap(texture, imgPath, flipY);
    return texture;
}

bool Function::OpenGLBase::BindTextureBuffer_Cubemap(GLuint& texture,
                                                     const char* imgPath,
                                                     bool flipY)
{
    int width, height, nrChannels;
    unsigned char* imgData = nullptr;
    func_LoadImage(imgPath, width, height, nrChannels, &imgData, flipY);
    if (imgData == nullptr)
        return false;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    // 加载纹理
    if (nrChannels == 1)
        glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RED, width, height, 0, GL_RED,
                     GL_UNSIGNED_BYTE, imgData);
    else if (nrChannels == 3)
        glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, imgData);
    else if (nrChannels == 4)
        glTexImage2D(GL_TEXTURE_CUBE_MAP, 0, GL_RGBA, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, imgData);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    if (nrChannels == 3)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {// 设置为GL_CLAMP_TO_EDGE
        // 能解决在有透明度的图片中使用插值时能看到明显的边框
        // OpenGL会对边缘的值和纹理下一个重复的值进行插值
        // 因为我们将它的环绕方式设置为了GL_REPEAT。
        // 这通常是没问题的，但是由于我们使用了透明值，
        // 纹理图像的顶部将会与底部边缘的纯色值进行插值。
        // 这样的结果是一个半透明的有色边框，你可能会看见它环绕着你的纹理四边形。
        // 要想避免这个，每当你alpha纹理的时候，请将纹理的环绕方式设置为GL_CLAMP_TO_EDGE：
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    // 设置过滤方式
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(imgData);
    return true;
}

bool Function::OpenGLBase::GetTexture(GLuint& texture,
                                      const std::string& imgPath)
{
    return GetTexture(texture, imgPath.c_str());
}

bool Function::OpenGLBase::GetTexture(GLuint& texture, const char* imgPath)
{
    std::filesystem::path fPath = std::filesystem::absolute(imgPath);
    return BindTextureBuffer(texture, fPath.string().c_str());
}

GLuint Function::OpenGLBase::GetTexture(const char* imgPath)
{
    GLuint texture;
    GetTexture(texture, imgPath);
    return texture;
}

GLuint Function::OpenGLBase::GetTexture(const std::string& imgPath)
{
    GLuint texture;
    GetTexture(texture, imgPath);
    return texture;
}

GLuint Function::OpenGLBase::BindBoxTexture(
    const std::string& basePath, const std::vector<std::string>& picFileName)
{
    std::vector<std::string> strs;
    for (const auto& fileName : picFileName)
    {
        strs.push_back(basePath + "/" + fileName);
    }
    return BindBoxTexture(strs);
}

GLuint
Function::OpenGLBase::BindBoxTexture(const std::vector<std::string>& picPaths)
{
    using namespace Function::LogSimple;
    if (picPaths.size() != 6)
    {
        GL_TRACE("need 6 pictures ");
        abort();
    }
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannel;
    unsigned char* data;
    int offset = 0;
    for (const auto& picPath : picPaths)
    {
        func_LoadImage(picPath.c_str(), width, height, nrChannel, &data);
        // GL_TRACE("PicPath: " + picPath);
        if (data == nullptr)
        {
            GL_TRACE("Get Image Data failed");
            stbi_image_free(data);
            abort();
        }
        if (nrChannel == 3)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + offset++, 0, GL_RGB, width,
                         height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        if (nrChannel == 4)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + offset++, 0, GL_RGBA, width,
                         height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}

void Function::OpenGLMath::SetVec2(const GLuint& sp,
                                   const std::string uniformStr,
                                   const glm::vec2& val)
{
    return Function::OpenGLMath::SetVec2(sp, uniformStr.c_str(), val);
}
void Function::OpenGLMath::SetVec2(const GLuint& sp, const char* uniformStr,
                                   const glm::vec2& val)
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform2fv(spu, 1, glm::value_ptr(val));
    return;
}

void Function::OpenGLMath::SetVec3(const GLuint& sp,
                                   const std::string uniformStr,
                                   const glm::vec3& val)
{
    return Function::OpenGLMath::SetVec3(sp, uniformStr.c_str(), val);
}
void Function::OpenGLMath::SetVec3(const GLuint& sp, const char* uniformStr,
                                   const glm::vec3& val)
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform3fv(spu, 1, glm::value_ptr(val));
    return;
}

void Function::OpenGLMath::SetVec4(const GLuint& sp,
                                   const std::string uniformStr,
                                   const glm::vec4& val)
{
    return Function::OpenGLMath::SetVec4(sp, uniformStr.c_str(), val);
}
void Function::OpenGLMath::SetVec4(const GLuint& sp, const char* uniformStr,
                                   const glm::vec4& val)
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform4fv(spu, 1, glm::value_ptr(val));
    return;
}

void Function::OpenGLMath::SetBool(const GLuint& sp,
                                   const std::string uniformStr,
                                   const bool& val)
{
    return SetInt(sp, uniformStr.c_str(), val);
}

void Function::OpenGLMath::SetInt(const GLuint& sp,
                                  const std::string uniformStr,
                                  const int& val)
{
    return SetInt(sp, uniformStr.c_str(), val);
}
void Function::OpenGLMath::SetInt(const GLuint& sp, const char* uniformStr,
                                  const int& val)
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform1i(spu, val);
    return;
}

void Function::OpenGLMath::SetFloat(const GLuint& sp,
                                    const std::string uniformStr,
                                    const float& val)
{
    return Function::OpenGLMath::SetFloat(sp, uniformStr.c_str(), val);
}
void Function::OpenGLMath::SetFloat(const GLuint& sp, const char* uniformStr,
                                    const float& val)
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform1f(spu, val);
    return;
}

void Function::OpenGLMath::SetMat4(const GLuint& sp,
                                   const std::string uniformStr,
                                   const glm::mat4& val)
{
    return SetMat4(sp, uniformStr.c_str(), val);
}
void Function::OpenGLMath::SetMat4(const GLuint& sp, const char* uniformStr,
                                   const glm::mat4& val)
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniformMatrix4fv(spu, 1, false, glm::value_ptr(val));
    return;
}

// 矩阵乘法实现在 Note/OpenGL/Pic/矩阵乘法.jpg
Function::mat Function::OpenGLMath::LookAt(const std::vector<float>& position,
                                           const std::vector<float>& target,
                                           const std::vector<float>& worldUp)
{
    if (position.size() != 3 || target.size() != 3 || worldUp.size() != 3)
        return Function::mat(0);
    glm::vec3 pos = glm::vec3(position[0], position[1], position[2]);
    glm::vec3 tar = glm::vec3(target[0], target[1], target[2]);
    glm::vec3 wUp = glm::vec3(worldUp[0], worldUp[1], worldUp[2]);
    glm::vec3 zAxis = glm::normalize(pos - tar);
    glm::vec3 yAxis = glm::normalize(glm::cross(zAxis, glm::normalize(wUp)));
    glm::vec3 xAxis = glm::cross(zAxis, yAxis);

    glm::mat4 translation = glm::mat4(1.0);
    translation[3][0] = -position[0];
    translation[3][1] = -position[1];
    translation[3][2] = -position[2];

    glm::mat4 rotation = glm::mat4(1.0);

    rotation[0][0] = xAxis.x;// First column, first row
    rotation[1][0] = xAxis.y;
    rotation[2][0] = xAxis.z;
    rotation[0][1] = yAxis.x;// First column, second row
    rotation[1][1] = yAxis.y;
    rotation[2][1] = yAxis.z;
    rotation[0][2] = zAxis.x;// First column, third row
    rotation[1][2] = zAxis.y;
    rotation[2][2] = zAxis.z;
    glm::mat4 m4LookAt = rotation * translation;
    Function::mat ret(16);
    ret[0][0] = m4LookAt[0][0];
    ret[0][1] = m4LookAt[0][1];
    ret[0][2] = m4LookAt[0][2];
    ret[0][3] = m4LookAt[0][3];
    ret[1][0] = m4LookAt[1][0];
    ret[1][1] = m4LookAt[1][1];
    ret[1][2] = m4LookAt[1][2];
    ret[1][3] = m4LookAt[1][3];
    ret[1][0] = m4LookAt[1][0];
    ret[1][1] = m4LookAt[1][1];
    ret[1][2] = m4LookAt[1][2];
    ret[1][3] = m4LookAt[1][3];
    ret[2][0] = m4LookAt[2][0];
    ret[2][1] = m4LookAt[2][1];
    ret[2][2] = m4LookAt[2][2];
    ret[2][3] = m4LookAt[2][3];
    return ret;
}

void Function::OpenGLBase::ShowGraphCardInfo()
{
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    std::cout << "厂商: " << vendor << "\n"
              << "渲染器: " << renderer << "\n"
              << "GLSL 版本: " << glslVersion << "\n"
              << "OpenGL 版本: " << version << std::endl;

    // 获取支持的扩展列表（需要先获取扩展数量）
    GLint numExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    std::cout << "支持的扩展 (" << numExtensions << " 个):\n";
    for (int i = 0; i < numExtensions; ++i)
    {
        std::cout << "  " << glGetStringi(GL_EXTENSIONS, i) << "\n";
    }
}

int Function::StencilTest::GetStencilTestEnum(const std::string& str)
{
    try
    {
        return stencilMap.at(str);
    }
    catch (const std::exception& ex)
    {
        std::cout << __FILE__ << ":" << __LINE__ << "  Exception: " << ex.what()
                  << std::endl;
        std::cout << __FILE__ << ":" << __LINE__
                  << "  StencilTestEnum Get Error: " << str << std::endl
                  << std::endl;

        Function::LinuxDebug::PrintStackTrace();
        // abort();
        throw ex;
    }
}

void Function::LinuxDebug::PrintStackTrace()
{
#ifdef __linux__
    PRINT_STACK_TRACE();
#else
    std::cout << __FILE__ << ":" << __LINE__ << " ";
    std::cout << "This function is not implement for windows." << "\t";
    std::cout << "Consider use Boost." << std::endl;
    // std::cout << boost::stacktrace::stacktrace() << std::endl;

#endif
}

void Function::GLDebug::PrintMat4(const glm::mat4& val)
{
    std::cout << std::fixed << std::setprecision(6);// 设置打印浮点的精度
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            std::cout << val[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "************************* mat4 End *************************"
              << std::endl;
}

void Function::GLDebug::PrintMat3(const glm::mat3& val)
{
    std::cout << std::fixed << std::setprecision(6);// 设置打印浮点的精度
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            std::cout << val[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "************************* mat3 End *************************"
              << std::endl;
}

void Function::GLDebug::PrintMat2(const glm::mat2& val)
{
    std::cout << std::fixed << std::setprecision(6);// 设置打印浮点的精度
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            std::cout << val[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "************************* mat2 End *************************"
              << std::endl;
}

void Function::GLDebug::PrintVec4(const glm::vec4& val)
{
    std::cout << std::fixed << std::setprecision(6);// 设置打印浮点的精度

    for (int j = 0; j < 4; ++j)
    {
        std::cout << val[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "************************* vec4 End *************************"
              << std::endl;
}

void Function::GLDebug::PrintVec3(const glm::vec3& val)
{
    std::cout << std::fixed << std::setprecision(6);// 设置打印浮点的精度

    for (int j = 0; j < 3; ++j)
    {
        std::cout << val[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "************************* vec3 End *************************"
              << std::endl;
}

void Function::GLDebug::PrintVec2(const glm::vec2& val)
{
    std::cout << std::fixed << std::setprecision(6);// 设置打印浮点的精度

    for (int j = 0; j < 2; ++j)
    {
        std::cout << val[j] << " ";
    }
    std::cout << std::endl;

    std::cout << "************************* vec2 End *************************"
              << std::endl;
}
