/**
 * @file       pbrFunc.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2025-08-16
 * @version    v1.0.0
 * @copyright  Copyright (c) 2025
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2025/08/16 Time: 12:52   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */

#ifndef __pbrFunc_H__
#define __pbrFunc_H__

#include "VertexManager.h"
#include "demo_Model.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <string>
#include <sys/types.h>
#include <vector>

#ifdef _WIN32
typedef unsigned int uint;
#endif
namespace IBL
{

static const glm::mat4 captureProjection =
    glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

static const glm::mat4 captureViews[] = {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, -1.0f, 0.0f))};

static const std::vector<glm::vec3> lightPositions = {
    glm::vec3(0, 10, 0),
    // glm::vec3(6, 10, 0),
    // glm::vec3(0, -15.4f, 15.0f),
    // glm::vec3(-15.0f, 0, 0),
    // glm::vec3(15.0f, 0, 0),
    // glm::vec3(0, 0, -15.0f),
    // glm::vec3(0, 0, 15.0f),
};
static const std::vector<glm::vec3> lightColors = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    // glm::vec3(300.0f, 300.0f, 300.0f),
    // glm::vec3(5000.0f, 5000.0f, 5000.0f),
    // glm::vec3(5000.0f, 5000.0f, 5000.0f),
    // glm::vec3(5000.0f, 5000.0f, 5000.0f),
    // glm::vec3(5000.0f, 5000.0f, 5000.0f),
    // glm::vec3(5000.0f, 5000.0f, 5000.0f)
    // glm::vec3(50.0f, 50.0f, 50.0f),
    // glm::vec3(50.0f, 50.0f, 50.0f),
    // glm::vec3(50.0f, 50.0f, 50.0f)
    // glm::vec3(1000.0f, 1000.0f, 1000.0f),
    // glm::vec3(1000.0f, 1000.0f, 1000.0f),
    // glm::vec3(1000.0f, 1000.0f, 1000.0f),
    // glm::vec3(1000.0f, 1000.0f, 1000.0f)
};

void DrawSphere(uint& sphereVAO, uint& indexCount, uint& vbo, uint& ebo);

namespace GenTexture
{
uint LoadHDRTexture(const std::string& picPath);
uint LoadHDRTexture(const char* picPath);
uint GenCubeMapTexture(uint size);
uint GenIrradianceTexture(uint size);
uint GenLUTTexture(uint size);

}// namespace GenTexture

namespace GenMap
{
// 加载带HDR的环境贴图作为天空盒子
//
// 0_hdrTex
void GenCubeMap(fbAttri& fb);

// PBR HDR立方体贴图
//
// 0_hdrTex 1_envMap
void GenEnvMap(fbAttri& fb);

// PBR 漫反射部分光照(立方体贴图卷积)
//
// 0_hdrTex 1_envMap 2_irradianceMap
void GenIrradianceMap(fbAttri& fb);

// PBR 预滤波HDR环境贴图
//
// 0_hdrTex 1_envMap 2_irradianceMap 3_prefilterMap
void GenPrefilterMap(fbAttri& fb);

// PBR LUT
//
// 0_hdrTex 1_envMap 2_irradianceMap 3_prefilterMap 4_brdfLutMap
void GenLUTMap(fbAttri& fb);

};// namespace GenMap

namespace ImGuiFuncWarpper
{
namespace ImguiCommon
{
void SyncParamater(glm::vec3& val, float input);
}

void ImGuiSlideThree(glm::vec3& val, const std::string& prefix,
                     const std::string& title, bool enableSyncBtn = false,
                     float min = -30, float max = 30);
void ImGuiScale(glm::vec3& val, const std::string& prefix,
                bool enableSyncBtn = false, float min = 0, float max = 1);
void ImGuiTranslate(glm::vec3& val, const std::string& prefix,
                    bool enableSyncBtn = false);
}// namespace ImGuiFuncWarpper

namespace Shadow
{
static constexpr GLuint sw = 1024, sh = 1024;
static constexpr GLfloat near_plane = 0.01f, far_plane = 64.0f;
void ShadowFrameBuff(fbAttri& fb);
void DepthMap(RenderPipeline& shader, const glm::vec3& lightPos);

void SwitchShadow(RenderPipeline& render);
void DrawRoad(RenderPipeline& render, demoModel& module, const Devices& device);
}// namespace Shadow

}// namespace IBL

#endif//__pbrFunc_H__
