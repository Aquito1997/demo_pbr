/**
 * @file       Vertex.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2025-06-22
 * @version    v1.0.0
 * @copyright  Copyright (c) 2025
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2025/06/22 Time: 16:13   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */

#ifndef __Vertex_H__
#define __Vertex_H__

#include "FuncBase.h"
#include "glm/fwd.hpp"
#include <Devices.h>
// #include <GL/glext.h>
#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>
#include <vector>

typedef class FrameBufferAttribute {
public:
  GLuint fbo;
  GLuint rbo;

  FrameBufferAttribute() : fbo(0), rbo(0) {};
  FrameBufferAttribute(GLuint fbo, GLuint rbo, std::vector<GLuint> vecTex)
      : fbo(fbo), rbo(rbo), mVecTex(vecTex) {};
  FrameBufferAttribute(GLuint fbo, GLuint rbo,
                       std::map<std::string, GLuint> vecTex)
      : fbo(fbo), rbo(rbo), mMapTex(vecTex) {};

  GLuint GetTexId(int index) {
    assert((int)mVecTex.size() > index);
    return mVecTex[index];
  }
  GLuint GetTexId(const std::string &texName) {
    assert(mMapTex.find(texName) != mMapTex.end());
    return mMapTex[texName];
  }

  void PushTex(int value) { mVecTex.push_back(std::forward<int>(value)); }

  void PushTex(std::pair<std::string, GLuint> value) {
    mMapTex.insert(std::forward<std::pair<std::string, GLuint>>(value));
  }

  size_t size() {
    if (mVecTex.size() > 0)
      return mVecTex.size();
    else
      return mMapTex.size();
  }

  ~FrameBufferAttribute() {
    if (mVecTex.size() > 0)
      glDeleteTextures(mVecTex.size(), mVecTex.data());
    else if (mMapTex.size() > 0)
      for (const auto &itor : mMapTex)
        glDeleteTextures(1, &itor.second);

    if (rbo)
      glDeleteRenderbuffers(1, &rbo);
    if (fbo)
      glDeleteFramebuffers(1, &fbo);

    mVecTex.clear();
    mMapTex.clear();
  };

private:
  std::vector<GLuint> mVecTex;
  std::map<std::string, GLuint> mMapTex;
} fbAttri;

typedef enum ModelType {
  MT_START = 0,
  MT_CUBE = 1,
  MT_TANGENT = 2,
  MT_QUAD = 3,
  MT_INSTANCE_MATRIX4 = 4,
  MT_END = 5
} LearnGL_mt;

typedef struct VAO_VBO {
  GLuint VAO;
  GLuint VBO;
  LearnGL_mt type;
} VAO_VBO;

typedef enum texType {
  TT_START = 0,
  TT_TEXTURE_2D = GL_TEXTURE_2D,
  TT_TEXTURE_CUBE_MAP = GL_TEXTURE_CUBE_MAP,
  TT_END = -1

} LearnGL_tt;

typedef struct TextureInfo {
  GLuint texId;
  LearnGL_tt texType;
  TextureInfo(GLuint texId, LearnGL_tt texType) {
    this->texId = texId;
    this->texType = texType;
  }
  TextureInfo() {};
} LearnGL_ti;

////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @class VertexManager
 * @brief 用于创建、管理VAO和VBO
 * @details 顶点对象数量和对应数量的buff
 */
class VertexManager {
private:
  std::vector<VAO_VBO> vao_vbo;

private:
  void SetABPair(const VAO_VBO &ab);

public:
  // 仅仅是绑定了vao和vbo.具体怎么传数据需要自己实现
  VertexManager() = default;
  VertexManager(const float *vboData, size_t vboSize);
  ~VertexManager();

  void Init(const float *vboData, size_t vboSize,
            LearnGL_mt type = LearnGL_mt::MT_CUBE);
  void Init(const std::vector<glm::mat4> &insMat,
            LearnGL_mt type = LearnGL_mt::MT_CUBE);
  void PushABPair(GLuint vao, GLuint vbo,
                  LearnGL_mt type = LearnGL_mt::MT_CUBE);
  const VAO_VBO &GetABPair(size_t index = 0);
  void PushABPair(VAO_VBO abp);

  // pos_3 normal_3 texcoords_2
  void SetVertexAttribute_cube(int index = 0);
  // pos_3 normal_3 texcoords_2 tangent_3 bitangent_3
  void SetVertexAttribute_tangent(int index = 0);
  // pos_3 texcoords_2
  void SetVertexAttribute_quad(int index = 0);
  void SetVertexAttribute_InstanceMatrix(int index = 0);

  size_t size() const;
  size_t VertexNumber() const;

  GLuint &VAO(size_t index);
  GLuint &VBO(size_t index);

  void Draw();
};

////////////////////////////////////////////////////////////////////////////////////////////
class RenderPipeline {
  VertexManager vm;
  std::vector<LearnGL_ti> vecTex;

public:
  GLuint sp;

public:
  RenderPipeline();
  // 创建时仅设置顶点
  RenderPipeline(const float *vboData, size_t vboSize);
  RenderPipeline(const std::vector<GLfloat> vecVertex);
  // 创建时仅设置着色器
  RenderPipeline(const std::string &vs, const std::string &fs);
  RenderPipeline(const std::string &vs, const std::string &gs,
                 const std::string &fs);
  ~RenderPipeline();

  size_t GetTextureCount() const;
  // 不支持 HDR
  void BindTexture(const std::string &texPath, LearnGL_tt texType,
                   bool flipY = true);
  // 不支持 HDR
  void BindTexture(GLuint id, LearnGL_tt texType);

  // 创建一个空的texture id
  void CreateTexture(LearnGL_tt texType);
  void PutTexId(GLuint index, GLuint id, LearnGL_tt texType);
  GLuint GetTexId(GLuint index = 0);
  GLuint GetTexType(GLuint index = 0);
  const LearnGL_ti &GetTextureInfo(GLuint index = 0) const;

  // vs & fs
  void LinkShaderProgram(const std::string &vs, const std::string &fs);
  // vs、gs、fs
  void LinkShaderProgram(const std::string &vs, const std::string &gs,
                         const std::string &fs);

  const GLuint &VAO(size_t index = 0);
  const GLuint &VBO(size_t index = 0);
  const VAO_VBO &GetABPair(size_t index = 0);
  void ActiveVertex(size_t index = 0);
  void PushABPair(const VAO_VBO &abp);
  void PushABPair(const GLuint vao, const GLuint vbo);
  void BindVertex(const float *vboData, size_t vboSize,
                  LearnGL_mt type = LearnGL_mt::MT_CUBE);
  void BindVertex(const std::vector<GLfloat> &vecVertex,
                  LearnGL_mt type = LearnGL_mt::MT_CUBE);
  void BindVertex(const std::vector<glm::mat4> &vecMat4,
                  LearnGL_mt type = LearnGL_mt::MT_CUBE);
  // glsl 的定点属性固定为:
  // pos_3,    normal_3,   texcoords_2,   tangent_3,   bitangent_3
  void VertexAttrib(int index, LearnGL_mt type);

  void PrintAllData();

  GLuint &Texture(size_t index = 0);
  void ActiveTexture() const;
  void DeActiveTexture();
  void SetTextureAttrib(const std::string uniformStr, const int &val) const;

  void use() const;

  void Draw();

  void SetBool(const std::string uniformStr, const bool &val) const;
  void SetInt(const std::string uniformStr, const int &val) const;
  void SetInt(const char *uniformStr, const int &val) const;
  void SetFloat(const std::string uniformStr, const float &val) const;
  void SetFloat(const char *uniformStr, const float &val) const;
  void SetVec2(const std::string uniformStr, const glm::vec2 &val) const;
  void SetVec2(const char *uniformStr, const glm::vec2 &val) const;
  void SetVec3(const std::string uniformStr, const glm::vec3 &val) const;
  void SetVec3(const char *uniformStr, const glm::vec3 &val) const;
  void SetVec4(const std::string uniformStr, const glm::vec4 &val) const;
  void SetVec4(const char *uniformStr, const glm::vec4 &val) const;
  void SetMat3(const std::string uniformStr, const glm::mat3 &val) const;
  void SetMat3(const char *uniformStr, const glm::mat3 &val) const;
  void SetMat4(const std::string uniformStr, const glm::mat4 &val) const;
  void SetMat4(const char *uniformStr, const glm::mat4 &val) const;
};
#endif