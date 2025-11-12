#include "demo_PBR.h"
#include "Devices.h"
#include "Function.h"
#include "GLDefine.h"
#include "ImGui/imgui.h"
#include "VertexManager.h"
#include "glad/glad.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "vertex.h"

// #include <GL/glext.h>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <vector>

#include "stb_image.h"

using namespace IBL::Shadow;

uint IBL::GenTexture::LoadHDRTexture(const char *picPath) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, nrComponents;
  float *data = stbi_loadf(picPath, &width, &height, &nrComponents, 0);
  unsigned int hdrTexture = 0;
  if (data != nullptr) {
    glGenTextures(1, &hdrTexture);
    glBindTexture(GL_TEXTURE_2D, hdrTexture);
    if (nrComponents == 3)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
                   GL_FLOAT, data);
    else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
                   GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Failed to load HDR image." << std::endl;
  }
  return hdrTexture;
}

uint IBL::GenTexture::GenCubeMapTexture(uint size) {
  GLuint envCubeMap = 0;
  glGenTextures(1, &envCubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
  for (size_t iCnt = 0; iCnt < 6; iCnt++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCnt, 0, GL_RGB16F, size,
                 size, 0, GL_RGB, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  // GL_LINEAR_MIPMAP_LINEAR
  // 在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER,
                  GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return envCubeMap;
}

uint IBL::GenTexture::GenIrradianceTexture(uint size) {
  uint irradianceTex = 0;
  glGenTextures(1, &irradianceTex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceTex);
  for (uint iCnt = 0; iCnt < 6; iCnt++)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCnt, 0, GL_RGB16F, size,
                 size, 0, GL_RGB, GL_FLOAT, nullptr);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return irradianceTex;
}

uint IBL::GenTexture::GenLUTTexture(uint size) {
  uint brdfLutTex = 0;
  glGenTextures(1, &brdfLutTex);
  glBindTexture(GL_TEXTURE_2D, brdfLutTex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, size, size, 0, GL_RG, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return brdfLutTex;
}

void IBL::DrawSphere(uint &sphereVAO, uint &indexCount, uint &vbo, uint &ebo) {
  if (sphereVAO == 0) {
    glGenVertexArrays(1, &sphereVAO);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 64;
    const unsigned int Y_SEGMENTS = 64;
    const float PI = 3.14159265359f;
    for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
      for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        float xSegment = (float)x / (float)X_SEGMENTS;
        float ySegment = (float)y / (float)Y_SEGMENTS;
        float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
        float yPos = std::cos(ySegment * PI);
        float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

        positions.push_back(glm::vec3(xPos, yPos, zPos));
        uv.push_back(glm::vec2(xSegment, ySegment));
        normals.push_back(glm::vec3(xPos, yPos, zPos));
      }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
      if (!oddRow) // even rows: y == 0, y == 2; and so on
      {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
          indices.push_back(y * (X_SEGMENTS + 1) + x);
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
      } else {
        for (int x = X_SEGMENTS; x >= 0; --x) {
          indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
          indices.push_back(y * (X_SEGMENTS + 1) + x);
        }
      }
      oddRow = !oddRow;
    }
    indexCount = static_cast<unsigned int>(indices.size());

    std::vector<float> data;
    for (unsigned int i = 0; i < positions.size(); ++i) {
      data.push_back(positions[i].x);
      data.push_back(positions[i].y);
      data.push_back(positions[i].z);
      if (normals.size() > 0) {
        data.push_back(normals[i].x);
        data.push_back(normals[i].y);
        data.push_back(normals[i].z);
      }
      if (uv.size() > 0) {
        data.push_back(uv[i].x);
        data.push_back(uv[i].y);
      }
    }
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0],
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 &indices[0], GL_STATIC_DRAW);
    static const unsigned int stride = (3 + 2 + 3) * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          (void *)(6 * sizeof(float)));
  }

  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

// 加载带HDR的环境贴图作为天空盒子
// 0 hdrtexture
void IBL::GenMap::GenCubeMap(fbAttri &fb) {
  glGenFramebuffers(1, &fb.fbo);
  glGenRenderbuffers(1, &fb.rbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, fb.rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, fb.rbo);

  // GLuint hdrTexture =
  // IBL::GenTexture::LoadHDRTexture("./res/tex/hdr/newport_loft.hdr");
  GLuint hdrTexture =
      IBL::GenTexture::LoadHDRTexture("./res/tex/hdr/horn-koppe_snow_4k.hdr");

  fb.PushTex(hdrTexture); // 0 hdrtexture
}

// 1 envCUbeMapTex
void IBL::GenMap::GenEnvMap(fbAttri &fb) {
  GLuint envCubeMapTex = IBL::GenTexture::GenCubeMapTexture(512);
  fb.PushTex(envCubeMapTex); // 0 hdrtexture   1 envCubeMapTex

  RenderPipeline equirectangular2CubeMap("./GLSL/CubeMap.vs",
                                         "./GLSL/Equirectangular2Cubemap.fs");
  equirectangular2CubeMap.BindVertex(_cubeVertices);
  equirectangular2CubeMap.VertexAttrib(0, LearnGL_mt::MT_CUBE);
  equirectangular2CubeMap.BindTexture(fb.GetTexId(0),
                                      LearnGL_tt::TT_TEXTURE_2D);
  equirectangular2CubeMap.SetTextureAttrib("equirectangularMap", 0);
  equirectangular2CubeMap.SetMat4("projection", IBL::captureProjection);

  equirectangular2CubeMap.use();
  equirectangular2CubeMap.ActiveVertex(0);
  equirectangular2CubeMap.ActiveTexture();
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  {
    glViewport(0, 0, 512, 512);
    for (uint iCnt = 0; iCnt < 6; iCnt++) {
      equirectangular2CubeMap.SetMat4("view", IBL::captureViews[iCnt]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCnt,
                             envCubeMapTex, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMapTex);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

// 2_irradianceMap
void IBL::GenMap::GenIrradianceMap(fbAttri &fb) {
  GLuint irrTex = IBL::GenTexture::GenIrradianceTexture(32);
  fb.PushTex(irrTex); // 0_hdrTex 1_envMap 2_irradianceMap
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, fb.rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

  // irradianceShader
  RenderPipeline irrShader("./GLSL/CubeMap.vs",
                           "./GLSL/IrradianceConvolution.fs");
  irrShader.BindVertex(_cubeVertices);
  irrShader.VertexAttrib(0, LearnGL_mt::MT_CUBE);
  irrShader.BindTexture(fb.GetTexId(1),
                        LearnGL_tt::TT_TEXTURE_CUBE_MAP); // fb[0] 报错1282
  irrShader.SetTextureAttrib("EnvMap", 0);
  irrShader.SetMat4("projection", IBL::captureProjection);
  irrShader.ActiveVertex();
  irrShader.ActiveTexture();
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  {
    glViewport(0, 0, 32, 32);
    for (uint iCnt = 0; iCnt < 6; iCnt++) {
      irrShader.SetMat4("view", IBL::captureViews[iCnt]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCnt, irrTex, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 3_prefilterMap
void IBL::GenMap::GenPrefilterMap(fbAttri &fb) {
  GLuint prefilterTex = GenTexture::GenCubeMapTexture(128);
  fb.PushTex(prefilterTex); // 0_hdrTex 1_envMap 2_irradianceMap 3_prefilterMap
  glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterTex);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  RenderPipeline prefilterShader("./GLSL/CubeMap.vs", "./GLSL/Prefilter.fs");
  prefilterShader.BindVertex(_cubeVertices);
  prefilterShader.VertexAttrib(0, LearnGL_mt::MT_CUBE);
  prefilterShader.BindTexture(fb.GetTexId(1),
                              LearnGL_tt::TT_TEXTURE_CUBE_MAP); // 1

  prefilterShader.SetTextureAttrib("EnvMap", 0);
  prefilterShader.SetMat4("projection", captureProjection);
  prefilterShader.ActiveVertex();
  prefilterShader.ActiveTexture();
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  {
    uint maxMipLevels = 5;
    for (uint mip = 0; mip < maxMipLevels; mip++) {
      const uint mipWidth = static_cast<uint>(128 * std::pow(0.5, mip));
      const uint mipHeight = static_cast<uint>(128 * std::pow(0.5, mip));
      glBindRenderbuffer(GL_RENDERBUFFER, fb.rbo);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth,
                            mipHeight);
      glViewport(0, 0, mipWidth, mipHeight);

      float roughness = (float)mip / (float)(maxMipLevels - 1);
      prefilterShader.SetFloat("roughness", roughness);
      for (uint iCnt = 0; iCnt < 6; iCnt++) {
        prefilterShader.SetMat4("view", IBL::captureViews[iCnt]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + iCnt,
                               prefilterTex, mip);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 36);
      }
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// 4_lutTex 2D 查找纹理
void IBL::GenMap::GenLUTMap(fbAttri &fb) {
  GLuint brdfLUTTex = GenTexture::GenLUTTexture(512);
  fb.PushTex(
      brdfLUTTex); // 0_hdrTex 1_envMap 2_irradianceMap 3_prefilterMap 4_lutTex

  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  glBindRenderbuffer(GL_RENDERBUFFER, fb.rbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         brdfLUTTex, 0);

  RenderPipeline brdfShader("./GLSL/BRDF.vs", "./GLSL/BRDF.fs");
  brdfShader.BindVertex(_quadVertex);
  brdfShader.VertexAttrib(0, LearnGL_mt::MT_QUAD);
  glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);
  {
    glViewport(0, 0, 512, 512);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    brdfShader.use();
    brdfShader.ActiveVertex();
    brdfShader.ActiveTexture();
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBL::Shadow::ShadowFrameBuff(fbAttri &fbo) {
  unsigned int depthCubemap;
  glGenTextures(1, &depthCubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
  for (unsigned int i = 0; i < 6; ++i)
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, sw,
                 sh, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // attach depth texture as FBO's depth buffer
  unsigned int depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  fbo.fbo = depthMapFBO;
  fbo.PushTex(depthCubemap);
}

void IBL::Shadow::DepthMap(RenderPipeline &shader, const glm::vec3 &lightPos) {
  shader.use();
  shader.SetMat4("model", BaseTypeLen::IMat4);
  static std::vector<glm::mat4> shadowTransforms;
  glm::mat4 shadowProj = glm::perspective(
      glm::radians(90.0f), (float)sw / (float)sh, near_plane, far_plane);
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos,
                               lightPos + glm::vec3(-1.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, 1.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos,
                               lightPos + glm::vec3(0.0f, -1.0f, 0.0f),
                               glm::vec3(0.0f, 0.0f, -1.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  shadowTransforms.push_back(
      shadowProj * glm::lookAt(lightPos,
                               lightPos + glm::vec3(0.0f, 0.0f, -1.0f),
                               glm::vec3(0.0f, -1.0f, 0.0f)));
  for (size_t xCnt = 0; xCnt < 6; xCnt++)
    Function::OpenGLMath::SetMat4(
        shader.sp, "shadowMatrices[" + std::to_string(xCnt) + "]",
        shadowTransforms[xCnt]);
  Function::OpenGLMath::SetFloat(shader.sp, "far_plane", far_plane);
  Function::OpenGLMath::SetVec3(shader.sp, "lightPos", lightPos);
}

void IBL::Shadow::SwitchShadow(RenderPipeline &render) {
  static bool shadow = true;
  ImGui::Checkbox("阴影开关", &shadow);
  Function::OpenGLMath::SetInt(render.sp, "shadows", shadow);
}
void IBL::Shadow::DrawRoad(RenderPipeline &render, demoModel &module,
                           const Devices &device) {
  render.use();
  render.ActiveTexture();
  SwitchShadow(render);

  glm::mat4 model = BaseTypeLen::IMat4;
  static glm::vec3 scale = glm::vec3(1);
  static glm::vec3 trans = glm::vec3(6.444, -0.5, -1.376);
  static glm::vec3 rotate = glm::vec3(-88, 0, -90);
  IBL::ImGuiFuncWarpper::ImGuiScale(scale, "roadS", false, -2, 2);
  IBL::ImGuiFuncWarpper::ImGuiScale(trans, "roadT", false, -180, 180);
  IBL::ImGuiFuncWarpper::ImGuiScale(rotate, "roadR", false, -180, 180);
  if (rotate.x != 0)
    model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
  if (rotate.y != 0)
    model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
  if (rotate.z != 0)
    model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));
  model = glm::scale(model, scale);
  model = glm::translate(model, trans);

  render.SetMat4("model", model);
  render.SetMat4("view", device.GetViewMatrix());
  render.SetMat4("projection", Function::OpenGLBase::perspective(
                                   device, BaseTypeLen::halfAspect));
  render.SetFloat("far_plane", far_plane);
  render.SetVec3("lightPos", IBL::lightPositions[0]);
  render.SetVec3("viewPos", device.Position);
  module.Draw(render.sp);
}

void IBL::ImGuiFuncWarpper::ImguiCommon::SyncParamater(glm::vec3 &val,
                                                       float input) {
  val.x = input;
  val.y = input;
  val.z = input;
}

void IBL::ImGuiFuncWarpper::ImGuiSlideThree(glm::vec3 &val,
                                            const std::string &prefix,
                                            const std::string &title,
                                            bool enableSyncBtn, float min,
                                            float max) {
  static bool sync = false;
  if (ImGui::CollapsingHeader((prefix + title).c_str(), true)) {
    ImGui::SetCursorPosX(30);
    if (ImGui::SliderFloat((prefix + std::string(".X")).c_str(), &val.x, min,
                           max))
      if (sync)
        ImguiCommon::SyncParamater(val, val.x);

    ImGui::SetCursorPosX(30);
    if (ImGui::SliderFloat((prefix + std::string(".Y")).c_str(), &val.y, min,
                           max))
      if (sync)
        ImguiCommon::SyncParamater(val, val.y);

    ImGui::SetCursorPosX(30);
    if (ImGui::SliderFloat((prefix + std::string(".Z")).c_str(), &val.z, min,
                           max))
      if (sync)
        ImguiCommon::SyncParamater(val, val.z);

    ImGui::NewLine();
    if (enableSyncBtn) {
      if (ImGui::Button((prefix + " Sync").c_str()))
        sync = !sync;
    }

    ImGui::SameLine(120);
    if (ImGui::Button((prefix + " 重置").c_str())) // 设置按钮的尺寸
      val = glm::vec3(1);
    ImGui::SameLine(180);
    if (ImGui::Button((prefix + " reset0").c_str())) // 设置按钮的尺寸
      val = glm::vec3(0);
    ImGui::Indent();
    ImGui::Unindent();
  }
}

void IBL::ImGuiFuncWarpper::ImGuiScale(glm::vec3 &val,
                                       const std::string &prefix,
                                       bool enableSyncBtn, float min,
                                       float max) {
  ImGuiSlideThree(val, prefix, "scale", enableSyncBtn, min, max);
}

void IBL::ImGuiFuncWarpper::ImGuiTranslate(glm::vec3 &val,
                                           const std::string &prefix,
                                           bool enableSyncBtn) {
  ImGuiSlideThree(val, prefix, "trans", enableSyncBtn);
}
