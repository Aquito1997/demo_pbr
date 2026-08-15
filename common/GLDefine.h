//
// Created by Aquito on 2025/3/20.
//

#ifndef WINOPENGL_ENUM_H
#define WINOPENGL_ENUM_H

#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <cstddef>
#include <map>
#include <string>

static const char* simsunPath = "./res/Font/simsun.ttf";

namespace BaseTypeLen
{
enum CHANNEL
{
    CHANNEL_START = 0,
    CHANNEL_RGB = 3,
    CHANNEL_RGBA = 4,
    CHANNEL_end = 5
};

constexpr static const size_t SCREEN_WIDTH = 3000;
constexpr static const size_t SCREEN_HEIGHT = 2400;
constexpr static const float aspect =
    (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
constexpr static const int halfHeight = SCREEN_HEIGHT / 2;
constexpr static const int halfWidth = SCREEN_WIDTH / 2;
constexpr static const float halfAspect = (float)halfWidth / (float)halfHeight;
constexpr static const int halfH = halfHeight;
constexpr static const int halfW = halfWidth;

constexpr static const int quaterW = halfWidth / 2;
constexpr static const int quaterH = halfWidth / 2;

constexpr static const size_t FltSize = sizeof(float);
constexpr static const size_t IntSize = sizeof(int);
constexpr static const size_t mat4Size = sizeof(glm::mat4);
constexpr static const size_t vec4Size = sizeof(glm::vec4);
constexpr static const glm::vec3 WhiteLight = {1.0, 1.0, 1.0};

constexpr static const glm::vec3 bcCamUp = glm::vec3(0.0f, 1.0f, 0.0f);
constexpr static const glm::vec3 vec3Zero = glm::vec3(0);
constexpr static const glm::mat4 IMat4 = glm::mat4(1);

}// namespace BaseTypeLen

enum class TimeUnit : unsigned long long int
{
    NANOSECOND = 1,       // 纳秒
    MICROSECOND = 1000,   // 微秒=1000纳秒
    MILLISECOND = 1000000,// 毫秒=1000微秒
    SECOND = 1000000000,  // 秒=1000毫秒
    MINUTE = 60 * SECOND, // 分=60秒
    HOUR = 60 * MINUTE,   // 时=60分
};

static const std::map<TimeUnit, std::string> timeUnit2Str = {
    {TimeUnit::NANOSECOND, "纳秒"},
    {TimeUnit::MICROSECOND, "微秒"},
    {TimeUnit::MILLISECOND, "毫秒"},
    {TimeUnit::SECOND, "妙"},
    {TimeUnit::MINUTE, "分"},
    {TimeUnit::HOUR, "时"},
};

#endif// WINOPENGL_ENUM_H
