#include "../inc/demo_Model.h"
#include "../inc/demo_Mesh.h"
#include "Devices.h"
#include "FuncBase.h"
#include "Function.h"
#include "GLDefine.h"
#include "ImGui/imgui.h"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/types.h"
#include "glad/glad.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

demoModel::demoModel(const std::string &path, bool flipY, bool pbrTex)
    : mFlipY(flipY), mLoadPBRTexture(pbrTex) {
  std::filesystem::path fs_path(path);
  fs_path = std::filesystem::absolute(path);
  if (!std::filesystem::exists(fs_path)) {
    GL_TRACE(std::string("Path:") + fs_path.string().c_str());
    GL_TRACE("demoModel path not exists");
    abort();
  }
  LoaddemoModel(fs_path.string());
}

demoModel::demoModel(const std::string &path, GLuint spid, bool flipY,
                     bool pbrTex)
    : mFlipY(flipY), mLoadPBRTexture(pbrTex) {
  std::filesystem::path fs_path(path);
  fs_path = std::filesystem::absolute(path);
  if (!std::filesystem::exists(fs_path)) {
    GL_TRACE("demoModel path not exists");
    abort();
  }
  glUseProgram(spid);
  LoaddemoModel(fs_path.string());
  RebindTexture();
}

demoModel::~demoModel() {
  for (const auto &tex : m_textures_loaded) {
    glDeleteTextures(1, &tex.id);
  }
  for (const auto &mesh : m_meshes) {
    glDeleteVertexArrays(1, &mesh.m_VAO);
    for (const auto &tex : mesh.m_demo_Textures) {
      glDeleteTextures(1, &tex.id);
    }
  }
}

void demoModel::SetStartIndex(size_t startIndex) {
  mStartIndex = startIndex;
  for (auto &mesh : m_meshes)
    mesh.SetStarIndex(mStartIndex);
};

void demoModel::Draw(const size_t &shader) {
  for (size_t iCnt = 0; iCnt < m_meshes.size(); iCnt++)
    m_meshes[iCnt].Draw(shader);
}

void demoModel::DrawArrays(const size_t &shader, const size_t &amount) {
  for (size_t iCnt = 0; iCnt < m_meshes.size(); iCnt++)
    m_meshes[iCnt].DrawArrays(shader, amount);
}

void demoModel::DrawArrays(const size_t &shader, const size_t &amount,
                           size_t index) {
  for (size_t iCnt = 0; iCnt < m_meshes.size(); iCnt++)
    m_meshes[iCnt].DrawArrays(shader, amount, index);
}

void demoModel::Draw(const size_t &shader, Devices &devices) {
  using namespace BaseTypeLen;
  glm::mat4 demoModel = glm::mat4(1.0f);
  glm::mat4 matView = devices.GetViewMatrix();
  glm::mat4 matProjection = glm::perspective(
      glm::radians(devices.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
      0.1f, 100.0f);

  {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // 选择要剔除的面
    glFrontFace(GL_CW);  // 逆时针的环绕顺序为正面
    static float scale = 1.003f;

    static char buffer[256] = {0}; // 存储输入内容的缓冲区
    ImGui::InputText("BackFace", buffer, sizeof(buffer));
    ImGui::SameLine();
    if (ImGui::Button("Confirm")) {
      scale = atof(buffer);
      std::cout << __FILE__ << ":" << __LINE__ << "Scale Size: " << scale
                << std::endl;
      std::cout << __FILE__ << ":" << __LINE__ << "buffer Size: " << buffer
                << std::endl;
    }
    demoModel = glm::translate(demoModel, glm::vec3(5.5f, 0.0f, 0.0f));
    demoModel = glm::scale(demoModel, glm::vec3(scale, scale, scale));

    glUseProgram(shader);
    Function::OpenGLMath::SetMat4(shader, "view", matView);
    Function::OpenGLMath::SetMat4(shader, "demoModel", demoModel);
    Function::OpenGLMath::SetMat4(shader, "projection", matProjection);
    Function::OpenGLMath::SetVec4(shader, "purgeColor",
                                  glm::vec4(0.8f, 0.2f, 0.0f, 1.0f));
    Draw(shader);
    glDisable(GL_CULL_FACE);
  }

  {
    glUseProgram(shader);
    demoModel = glm::translate(glm::mat4(1.0f), glm::vec3(5.5f, 0.0f, 0.0f));
    Function::OpenGLMath::SetMat4(shader, "demoModel", demoModel);
    Function::OpenGLMath::SetMat4(shader, "view", matView);
    Function::OpenGLMath::SetMat4(shader, "projection", matProjection);
    Function::OpenGLMath::SetVec4(shader, "purgeColor", glm::vec4(0));
    Draw(shader);
    glUseProgram(0);
  }
}

void demoModel::SearchMeshTextureType(const aiMaterial *mat) {
  for (auto &itor : assimpType) {
    // std::cout << "aiTexId: " << itor.second << std::endl;
    auto type = static_cast<aiTextureType>(itor.second);
    unsigned int textureCnt = mat->GetTextureCount(type);
    for (unsigned int i = 0; i < textureCnt; i++) {
      aiString path;
      if (mat->GetTexture(type, i, &path) == aiReturn_SUCCESS) {
        std::cout << "Texture Type: " << itor.first << " Path: " << path.C_Str()
                  << std::endl;
      }
    }
  }
}

void demoModel::LoaddemoModel(const std::string &path) {
  Assimp::Importer importer;
  // aiProcess_Triangulate:
  // 告诉Assimp，如果模型不是（全部）由三角形组成，它需要将模型所有的图元形状变换为三角形
  // aiProcess_GenSmoothNormals: 用于为导入的3D模型生成平滑的法线。
  // aiProcess_FlipUVs: 修正不同3D建模软件和图形API之间的UV坐标系差异
  // aiProcess_CalcTangentSpace: 为模型
  // 自动计算切线（Tangent）和副切线（Bitangent），
  //            以支持 法线贴图（Normal Mapping） 和 视差贴图（Parallax
  //            Mapping） 等高级渲染技术。
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
  // const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |
  // aiProcess_FlipUVs | aiProcess_CalcTangentSpace); const aiScene* scene =
  // importer.ReadFile(path, aiProcess_GenSmoothNormals | aiProcess_FlipUVs |
  // aiProcess_CalcTangentSpace);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    std::cout << __FILE__ << ":" << __LINE__
              << ": Error:Assimp::" << importer.GetErrorString() << std::endl;
    Function::LinuxDebug::PrintStackTrace();
    return;
  }
  if (path.find('/') != std::string::npos)
    m_directory = path.substr(0, path.find_last_of('/'));
  else
    m_directory = path.substr(0, path.find_last_of('\\'));

  ProcessNode(scene->mRootNode, scene);
}

void demoModel::ProcessNode(aiNode *node, const aiScene *scene) {
  for (size_t iCnt = 0; iCnt < node->mNumMeshes; iCnt++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[iCnt]];
    m_meshes.push_back(ProcessMesh(mesh, scene));
  }

  for (size_t iCnt = 0; iCnt < node->mNumChildren; iCnt++) {
    ProcessNode(node->mChildren[iCnt], scene);
  }
}

demo_Mesh demoModel::ProcessMesh(const aiMesh *mesh, const aiScene *scene) {
  assert(mesh && scene);
  std::vector<demo_Vertex> vecVertices;  // ^ 顶点
  std::vector<unsigned int> vecIndices;  // ^ 索引
  std::vector<demo_Texture> vecTextures; // ^ 纹理
  std::vector<demo_Material> vecMaterial;

  LoadVertices(vecVertices, mesh);
  LoadIndices(vecIndices, mesh);
  if (mLoadPBRTexture)
    LoadPBRTextures(vecTextures, mesh, scene);
  else
    LoadTextures(vecTextures, mesh, scene);
  LoadMaterial(vecMaterial, mesh, scene);
  return demo_Mesh(vecVertices, vecIndices, vecTextures, vecMaterial,
                   mLoadPBRTexture);
}

size_t demoModel::LoadVertices(std::vector<demo_Vertex> &retVal,
                               const aiMesh *mesh) {
  for (size_t iCnt = 0; iCnt < mesh->mNumVertices; iCnt++) {
    demo_Vertex vertex;
    glm::vec3 vector;
    vector.x = mesh->mVertices[iCnt].x;
    vector.y = mesh->mVertices[iCnt].y;
    vector.z = mesh->mVertices[iCnt].z;
    vertex.Position = vector;
    if (mesh->HasNormals()) {
      vector.x = mesh->mNormals[iCnt].x;
      vector.y = mesh->mNormals[iCnt].y;
      vector.z = mesh->mNormals[iCnt].z;
      vertex.Normal = vector;
    }
    if (mesh->mTextureCoords[0]) {
      glm::vec2 vec;
      vec.x = mesh->mTextureCoords[0][iCnt].x;
      vec.y = mesh->mTextureCoords[0][iCnt].y;
      vertex.TexCoords = vec;

      if (mesh->HasTangentsAndBitangents()) {
        vector.x = mesh->mTangents[iCnt].x;
        vector.y = mesh->mTangents[iCnt].y;
        vector.z = mesh->mTangents[iCnt].z;
        vertex.Tangent = vector;

        vector.x = mesh->mBitangents[iCnt].x;
        vector.y = mesh->mBitangents[iCnt].y;
        vector.z = mesh->mBitangents[iCnt].z;
        vertex.Bitangent = vector;
      } else {
        vertex.Tangent = glm::vec3(0);
        vertex.Bitangent = glm::vec3(0);
      }
    } else {
      vertex.TexCoords = glm::vec2(0.0f, 0.0f);
      vertex.Tangent = glm::vec3(0);
      vertex.Bitangent = glm::vec3(0);
    }
    retVal.push_back(vertex);
  }
  return retVal.size();
}

size_t demoModel::LoadIndices(std::vector<unsigned int> &retVal,
                              const aiMesh *mesh) {
  for (size_t iCnt = 0; iCnt < mesh->mNumFaces; iCnt++) {
    aiFace face = mesh->mFaces[iCnt];
    for (size_t jCnt = 0; jCnt < face.mNumIndices; jCnt++)
      retVal.push_back(face.mIndices[jCnt]);
  }
  return retVal.size();
}

size_t demoModel::LoadTextures(std::vector<demo_Texture> &retVal,
                               const aiMesh *mesh, const aiScene *scene) {
  const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<demo_Texture> diffuseMaps, specularMaps, normalMaps, ambients,
      emissives, opacities, shininess;

  normalMaps = this->LoadMaterialTextures(material, aiTextureType_NORMALS,
                                          "texture_normal"); // map_bump
  opacities = this->LoadMaterialTextures(material, aiTextureType_OPACITY,
                                         "texture_opacity"); // map_d
  ambients = this->LoadMaterialTextures(material, aiTextureType_AMBIENT,
                                        "texture_ambient"); // map_ka
  diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE,
                                           "texture_diffuse"); // map_kd
  emissives = this->LoadMaterialTextures(material, aiTextureType_EMISSIVE,
                                         "texture_emissive"); // map_ke
  specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR,
                                            "texture_specular"); // map_ks
  shininess = this->LoadMaterialTextures(material, aiTextureType_SHININESS,
                                         "texture_shininess"); // map_Ns

  retVal.insert(retVal.end(), normalMaps.begin(), normalMaps.end());
  retVal.insert(retVal.end(), opacities.begin(), opacities.end());
  retVal.insert(retVal.end(), ambients.begin(), ambients.end());
  retVal.insert(retVal.end(), diffuseMaps.begin(), diffuseMaps.end());
  retVal.insert(retVal.end(), emissives.begin(), emissives.end());
  retVal.insert(retVal.end(), specularMaps.begin(), specularMaps.end());
  retVal.insert(retVal.end(), shininess.begin(), shininess.end());

  return retVal.size();
}

size_t demoModel::LoadPBRTextures(std::vector<demo_Texture> &retVal,
                                  const aiMesh *mesh, const aiScene *scene) {
  const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
  std::vector<demo_Texture> albedoMap, aoMap, normalMap, metallicMap, roughMap;

  albedoMap = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE,
                                         "albedo"); // map_Kd
  aoMap = this->LoadMaterialTextures(material, aiTextureType_AMBIENT,
                                     "ao"); // map_Ka (AO)
  normalMap = this->LoadMaterialTextures(material, aiTextureType_HEIGHT,
                                         "normal"); // map_Bump
  metallicMap = this->LoadMaterialTextures(material, aiTextureType_EMISSIVE,
                                           "metallic"); // map_Ke (金属度)
  roughMap = this->LoadMaterialTextures(material, aiTextureType_SHININESS,
                                        "roughness"); // map_Ns (粗糙度)

  retVal.insert(retVal.end(), albedoMap.begin(), albedoMap.end());
  retVal.insert(retVal.end(), aoMap.begin(), aoMap.end());
  retVal.insert(retVal.end(), normalMap.begin(), normalMap.end());
  retVal.insert(retVal.end(), metallicMap.begin(), metallicMap.end());
  retVal.insert(retVal.end(), roughMap.begin(), roughMap.end());

  return retVal.size();
}

std::vector<demo_Texture>
demoModel::LoadMaterialTextures(const aiMaterial *material, aiTextureType type,
                                const std::string &typeName) {
  std::vector<demo_Texture> vecTextures;
  const auto &textureCnt = material->GetTextureCount(type);
  for (unsigned int iCnt = 0; iCnt < textureCnt; iCnt++) {
    aiString str;
    material->GetTexture(type, iCnt, &str);

    bool skip = false;
    for (const auto &tex : m_textures_loaded) {
      if (!std::strcmp(tex.path.data(), str.C_Str()) && tex.type == typeName) {
        vecTextures.push_back(tex);
        skip = true;
        break;
      }
    }
    if (skip == false) {
      GLuint tID = TextureFromFile(str.C_Str(), this->m_directory, mFlipY);
      demo_Texture texture(tID, typeName, str.C_Str());
      vecTextures.push_back(texture);
      m_textures_loaded.push_back(texture);
    }
  }
  return vecTextures;
}

size_t demoModel::LoadMaterial(std::vector<demo_Material> &retVal,
                               const aiMesh *mesh, const aiScene *scene) {
  assert(scene);
  assert(mesh);

  float attri;
  demo_Color3D color;
  const aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

  // color
  if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS) {
    demo_Material mat(true, demo_materialColor("Ka", color));
    retVal.push_back(mat);
  }
  if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS) {
    demo_Material mat(true, demo_materialColor("Kd", color));
    retVal.push_back(mat);
  }
  if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS) {
    demo_Material mat(true, demo_materialColor("Ks", color));
    retVal.push_back(mat);
  }
  if (material->Get(AI_MATKEY_COLOR_EMISSIVE, color) == AI_SUCCESS) {
    demo_Material mat(true, demo_materialColor("Ke", color));
    retVal.push_back(mat);
  }

  // attri
  if (material->Get(AI_MATKEY_SHININESS, attri) == AI_SUCCESS) {
    demo_Material mat(false, demo_materialAttri("Ns", attri));
    retVal.push_back(mat);
  }
  if (material->Get(AI_MATKEY_REFLECTIVITY, attri) == AI_SUCCESS) {
    demo_Material mat(false, demo_materialAttri("Ni", attri));
    retVal.push_back(mat);
  }
  if (material->Get(AI_MATKEY_OPACITY, attri) == AI_SUCCESS) {
    demo_Material mat(false, demo_materialAttri("d", attri));
    retVal.push_back(mat);
  }

  return retVal.size();
}

void demoModel::BindVertexInstance(const std::vector<glm::mat4> &insMat) {
  if (insMat.size() == 0) {
    Function::LinuxDebug::PrintStackTrace();
    abort();
  }
  GLuint amount = insMat.size();
  glGenBuffers(1, &instanceMatrix);
  glBindBuffer(GL_ARRAY_BUFFER, instanceMatrix);
  // glBufferData(GL_ARRAY_BUFFER, amount * BaseTypeLen::mat4Size,
  // insMat.data(), GL_STATIC_DRAW);、
  glBufferData(GL_ARRAY_BUFFER, amount * BaseTypeLen::mat4Size, insMat.data(),
               GL_STREAM_DRAW);

  for (const auto &mesh : m_meshes) {
    for (size_t iCnt = 0; iCnt < amount; iCnt++) {
      glBindVertexArray(mesh.m_VAO);
      glEnableVertexAttribArray(7);
      glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * BaseTypeLen::vec4Size,
                            (void *)(0 * BaseTypeLen::vec4Size));

      glEnableVertexAttribArray(8);
      glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * BaseTypeLen::vec4Size,
                            (void *)(1 * BaseTypeLen::vec4Size));

      glEnableVertexAttribArray(9);
      glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, 4 * BaseTypeLen::vec4Size,
                            (void *)(2 * BaseTypeLen::vec4Size));

      glEnableVertexAttribArray(10);
      glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE,
                            4 * BaseTypeLen::vec4Size,
                            (void *)(3 * BaseTypeLen::vec4Size));

      glVertexAttribDivisor(7, 1);
      glVertexAttribDivisor(8, 1);
      glVertexAttribDivisor(9, 1);
      glVertexAttribDivisor(10, 1);

      glBindVertexArray(0);
    }
  }
}

void demoModel::RebindTexture() {
  std::vector<demo_Texture> tmpTexture;
  for (demo_Mesh &mesh : this->m_meshes) {
    for (demo_Texture &texture : mesh.m_demo_Textures) {
      bool useTmpData = false;
      for (auto &tmpTex : tmpTexture) {
        if (tmpTex.path == texture.path) {
          texture.id = tmpTex.id;
          useTmpData = 1;
          break;
        }
      }
      if (!useTmpData) {
        texture.id = demoModel::ReBindTextureFromFile(
            texture.path, this->m_directory, mFlipY);
        tmpTexture.push_back(texture);
      }
    }
  }
}

unsigned int demoModel::ReBindTextureFromFile(const std::string &path,
                                              const std::string &dir,
                                              bool flipY) {
  std::string filename = std::string(path);
  filename = dir + "/" + filename;

  GLuint textureID = 0;
  glGenTextures(1, &textureID);
  if (textureID == 0) {
    std::cout << __FILE__ << ":" << __LINE__ << ": Texture ID is 0\t"
              << "Texture: " << path << std::endl;
    return textureID;
  }

  int width, height, nrComponents;
  unsigned char *data = nullptr;
  Function::OpenGLBase::func_LoadImage(filename.c_str(), width, height,
                                       nrComponents, &data, flipY);

  if (data) {
    GLenum format = 0;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // 设置环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 设置过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    stbi_image_free(data);
  } else {
    std::cout << __FILE__ << ":" << __LINE__ << "  "
              << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }
  return textureID;
}

unsigned int demoModel::TextureFromFile(const char *path,
                                        const std::string &directory,
                                        bool flipY) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrChannels;
  unsigned char *data = nullptr;
  Function::OpenGLBase::func_LoadImage(filename.c_str(), width, height,
                                       nrChannels, &data, flipY);

  if (data) {
    GLenum format = 0;
    if (nrChannels == 1)
      format = GL_RED;
    else if (nrChannels == 3)
      format = GL_RGB;
    else if (nrChannels == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
