#ifndef __Demo_demoModel_H__
#define __Demo_demoModel_H__

#include "Devices.h"
#include "assimp/material.h"
#include "demo_Mesh.h"

#include <GLDefine.h>
#include <atomic>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/texture.h>


static const std::map<std::string, int> assimpType = {
    {"aiTextureType_DIFFUSE", 1},
    {"aiTextureType_SPECULAR", 2},
    {"aiTextureType_AMBIENT", 3},
    {"aiTextureType_EMISSIVE", 4},
    {"aiTextureType_HEIGHT", 5},
    {"aiTextureType_NORMALS", 6},
    {"aiTextureType_SHININESS", 7},
    {"aiTextureType_OPACITY", 8},
    {"aiTextureType_DISPLACEMENT", 9},
    {"aiTextureType_LIGHTMAP", 10},
    {"aiTextureType_REFLECTION", 11},
    {"aiTextureType_BASE_COLOR", 12},
    {"aiTextureType_NORMAL_CAMERA", 13},
    {"aiTextureType_EMISSION_COLOR", 14},
    {"aiTextureType_METALNESS", 15},
    {"aiTextureType_DIFFUSE_ROUGHNESS", 16},
    {"aiTextureType_AMBIENT_OCCLUSION", 17},
    {"aiTextureType_UNKNOWN", 18},
    {"aiTextureType_SHEEN", 19},
    {"aiTextureType_CLEARCOAT", 20},
    {"aiTextureType_TRANSMISSION", 21},
    {"aiTextureType_MAYA_BASE", 22},
    {"aiTextureType_MAYA_SPECULAR", 23},
    {"aiTextureType_MAYA_SPECULAR_COLOR", 24},
    {"aiTextureType_MAYA_SPECULAR_ROUGHNESS", 25},
};

class demoModel
{
public:
    enum class MoudelType
    {
        OBJ = 0,
        GLB = 1,
        FBX = 2,
        STL = 3,
        DAE = 4,
        GLTF = 5,
        PLY = 6,
        BLENDER = 7,
        UNKNOW = 8
    };


    std::vector<demo_Texture> m_textures_loaded;// 已经加载了的纹理
    std::vector<demo_Mesh> m_meshes;            // 网格
    std::string m_directory;                    //
    bool m_gammaCorrection;                     //

    demoModel(const std::string& demoModelPath, bool pbrTex = false);
    demoModel(const std::string& demoModelPath, bool flipY = false, bool pbrTex = false);
    demoModel(const std::string& demoModelPath, GLuint spid, bool flipY = false, bool pbrTex = false);
    ~demoModel();
    // demoModel(bool gamma = false);

    void SearchMeshTextureType(const aiMaterial* material);

    void LoaddemoModel(const std::string& path);

    void Draw(const size_t& shader);
    /**
     * @brief 实例化时,绘制多个模型
     * 
     * @param spID shader program ID
     * @param amount 实例化时绘制模型的个数
     */


    void DrawArrays(const size_t& shader, const size_t& amount);
    /**
     * @brief 实例化时,绘制多个模型
     * 
     * @param spID shader program ID
     * @param amount 实例化时绘制模型的个数
     * @param index GL_TEXTURE0 + index 作为绑定纹理的起始位置.
     */

    void SetStartIndex(size_t startIndex);

    void DrawArrays(const size_t& shader, const size_t& amount, size_t index);
    //章节12 BackFace 效果的实现
    void Draw(const size_t& shader, Devices& camera);

    /**
     * @brief 加载模型时并没有真正的绑定纹理，在此处绑定纹理
    */
    void RebindTexture();

    /**
     * @brief 设置实例化的 顶点着色器 的属性
     * 
     * @param demoModelMatrix 
     */
    void BindVertexInstance(const std::vector<glm::mat4>& insMat);
    static unsigned int ReBindTextureFromFile(const std::string& path, const std::string& dir, bool flipy = false);
    static unsigned int TextureFromFile(const char* path, const std::string& directory, bool flipY = false);


private:
    void ProcessNode(aiNode* node, const aiScene* scene);
    demo_Mesh ProcessMesh(const aiMesh* mesh, const aiScene* scene);
    size_t LoadVertices(std::vector<demo_Vertex>& retVal, const aiMesh* mesh);
    size_t LoadIndices(std::vector<unsigned int>& retVal, const aiMesh* mesh);
    size_t LoadTextures(std::vector<demo_Texture>& retVal, const aiMesh* mesh, const aiScene* scene);
    size_t LoadPBRTextures(std::vector<demo_Texture>& retVal, const aiMesh* mesh, const aiScene* scene);
    size_t LoadMaterial(std::vector<demo_Material>& retVal, const aiMesh* mesh, const aiScene* scene);

    std::vector<demo_Texture> LoadMaterialTextures(const aiMaterial* material, aiTextureType type, const std::string& typeName);


private:
    bool mFlipY;
    size_t mStartIndex;
    bool mLoadPBRTexture;
    GLuint instanceMatrix{};// 实例化设置定点属性所需要的点
};

#endif//__demoModel_H__