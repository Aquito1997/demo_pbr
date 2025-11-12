#include "../inc/demo_Mesh.h"
#include "FuncBase.h"
#include "Function.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include <GL/gl.h>
#include <cstddef>
#include <string>

#ifdef __WIN32
#include <winscard.h>
#endif

demo_Mesh::demo_Mesh(std::vector<demo_Vertex> vertices,
                     std::vector<unsigned int> indices, std::vector<demo_Texture> demo_Textures, bool pbrTex)
{
    printed = true;
    mPbrTex = pbrTex;
    m_vertices = vertices;
    m_indices = indices;
    m_demo_Textures = demo_Textures;
    SetupMesh();
}

demo_Mesh::demo_Mesh(std::vector<demo_Vertex> vertices, std::vector<unsigned int> indices,
                     std::vector<demo_Texture> demo_Textures, std::vector<demo_Material> materials, bool pbrTex)
{
    printed = true;
    mPbrTex = pbrTex;
    m_vertices = vertices;
    m_indices = indices;
    m_demo_Textures = demo_Textures;
    m_materials = materials;
    SetupMesh();
}


void demo_Mesh::SetTexture(const size_t& spID)
{
    if (m_demo_Textures.size() < 1) return;

    size_t diffuseCnt = 1;
    size_t specularCnt = 1;
    size_t normalCnt = 1;
    size_t shininessCnt = 1;
    size_t opacityCnt = 1;
    size_t ambientCnt = 1;
    size_t emissiveCnt = 1;


    Function::OpenGLMath::SetInt(spID, "matel.useOpac", 0);
    Function::OpenGLMath::SetInt(spID, "matel.useDiff", 0);
    Function::OpenGLMath::SetInt(spID, "matel.useAmbt", 0);
    Function::OpenGLMath::SetInt(spID, "matel.useSpec", 0);
    Function::OpenGLMath::SetInt(spID, "matel.useShin", 0);
    Function::OpenGLMath::SetInt(spID, "matel.useNorm", 0);
    Function::OpenGLMath::SetInt(spID, "matel.useEmis", 0);


    static std::string uniStr;
    static const std::string name_("matel.");
    for (size_t iCnt = 0; iCnt < m_demo_Textures.size(); iCnt++)
    {
        std::string number;
        std::string name = m_demo_Textures[iCnt].type;
        glActiveTexture(GL_TEXTURE0 + iCnt);
        if (name == "texture_normal")
        {
            uniStr = (name_ + "normal" + std::to_string(normalCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useNorm", 1);
        }
        else if (name == "texture_opacity")
        {
            uniStr = (name_ + "opacity" + std::to_string(opacityCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useOpac", 1);
        }
        else if (name == "texture_diffuse")
        {
            uniStr = (name_ + "diffuse" + std::to_string(diffuseCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useDiff", 1);
        }
        else if (name == "texture_ambient")
        {
            uniStr = (name_ + "ambient" + std::to_string(ambientCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useAmbt", 1);
        }
        else if (name == "texture_specular")
        {
            uniStr = (name_ + "specular" + std::to_string(specularCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useSpec", 1);
        }
        else if (name == "texture_shininess")
        {
            uniStr = (name_ + "shininess" + std::to_string(shininessCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useShin", 1);
        }
        else if (name == "texture_emissive")
        {
            uniStr = (name_ + "emissive" + std::to_string(emissiveCnt++));
            Function::OpenGLMath::SetInt(spID, "matel.useEmis", 1);
        }
        else
            continue;

        Function::OpenGLMath::SetInt(spID, uniStr.c_str(), iCnt);
        glBindTexture(GL_TEXTURE_2D, m_demo_Textures[iCnt].id);
    }

    if (m_demo_Textures.size() > 0)
        glUniform1i(glGetUniformLocation(spID, "matel.useTex"), 1);
    else
        glUniform1i(glGetUniformLocation(spID, "matel.useTex"), 0);
}


void demo_Mesh::SetPbrTexture(const size_t& spID)
{
    if (m_demo_Textures.size() < 1) return;
    static const std::vector<std::string> shaderName = {
        "albedoMap", "aoMap", "normalMap", "metallicMap", "roughnessMap"};
    for (size_t iCnt = 0; iCnt < m_demo_Textures.size(); iCnt++)
    {
        Function::OpenGLMath::SetInt(spID, shaderName[iCnt].c_str(), iCnt + mStartIndex);
        glActiveTexture(GL_TEXTURE0 + iCnt + mStartIndex);
        glBindTexture(GL_TEXTURE_2D, m_demo_Textures[iCnt].id);
    }
    printed = false;
}


void demo_Mesh::SetMaterial(const size_t& spID)
{
    if ((int)m_materials.size() < 0) return;

    glUniform1i(glGetUniformLocation(spID, "matel.useKa"), 0);
    glUniform1i(glGetUniformLocation(spID, "matel.useKd"), 0);
    glUniform1i(glGetUniformLocation(spID, "matel.useKs"), 0);
    glUniform1i(glGetUniformLocation(spID, "matel.useNs"), 0);
    glUniform1i(glGetUniformLocation(spID, "matel.useNi"), 0);
    glUniform1i(glGetUniformLocation(spID, "matel.used"), 0);

    static const std::string basicStr = "matel.";
    std::string suffix;
    for (auto& matel : m_materials)
    {
        if (matel.m_isColor)
        {
            if (matel.m_color.first == "Ka")
            {
                suffix = "ka";
                glUniform1i(glGetUniformLocation(spID, "matel.useKa"), 1);
            }
            else if (matel.m_color.first == "Kd")
            {
                suffix = "kd";
                glUniform1i(glGetUniformLocation(spID, "matel.useKd"), 1);
            }
            else if (matel.m_color.first == "Ks")
            {
                suffix = "ks";
                glUniform1i(glGetUniformLocation(spID, "matel.useKs"), 1);
            }
            else
                continue;
            glUniform3fv(glGetUniformLocation(spID, std::string(basicStr + suffix).c_str()),
                         1, glm::value_ptr(matel.m_color.second.data()));
        }
        else
        {
            if (matel.m_attri.first == std::string("Ns"))
            {
                suffix = "Ns";
                glUniform1i(glGetUniformLocation(spID, "matel.useNs"), 1);
            }
            else if (matel.m_attri.first == std::string("Ni"))
            {
                suffix = "Ni";
                glUniform1i(glGetUniformLocation(spID, "matel.useNi"), 1);
            }
            else if (matel.m_attri.first == std::string("d"))
            {
                suffix = "d";
                glUniform1i(glGetUniformLocation(spID, "matel.used"), 1);
            }
            else
            {
                continue;
            }
            glUniform1f(glGetUniformLocation(spID, std::string(basicStr + suffix).c_str()),
                        matel.m_attri.second);
        }
    }
}


void demo_Mesh::Draw(const size_t& spID)
{
    glUseProgram(spID);
    if (mPbrTex)
        if (mStartIndex == 0)
            std::cout << "Not set texture start index yet" << std::endl;
        else
            SetPbrTexture(spID);
    else
        SetTexture(spID);
    SetMaterial(spID);

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<size_t>(m_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


void demo_Mesh::DrawArrays(const size_t& spID, const size_t& amount)
{
    size_t diffuseNr = 1;
    size_t specularNr = 1;
    size_t normalNr = 1;
    size_t heightNr = 1;

    for (size_t iCnt = 0; iCnt < m_demo_Textures.size(); iCnt++)
    {
        glActiveTexture(GL_TEXTURE0 + iCnt);
        std::string number;
        std::string name = m_demo_Textures[iCnt].type;
        if (name == "Texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "Texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "Texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "Texture_height")
            number = std::to_string(heightNr++);
        else
            continue;

        glUniform1i(glGetUniformLocation(spID, (name + number).c_str()), iCnt);
        glBindTexture(GL_TEXTURE_2D, m_demo_Textures[iCnt].id);
    }
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<size_t>(m_indices.size()), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}


void demo_Mesh::DrawArrays(const size_t& spID, const size_t& amount, size_t index)
{
    size_t diffuseNr = 1;
    size_t specularNr = 1;
    size_t normalNr = 1;
    size_t heightNr = 1;

    for (size_t iCnt = 0; iCnt < m_demo_Textures.size(); iCnt++)
    {
        std::string number;
        std::string name = m_demo_Textures[iCnt].type;
        if (name == "Texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "Texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "Texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "Texture_height")
            number = std::to_string(heightNr++);
        else
            continue;

        glUniform1i(glGetUniformLocation(spID, (name + number).c_str()), index + iCnt);
        glActiveTexture(GL_TEXTURE0 + index + iCnt);
        glBindTexture(GL_TEXTURE_2D, m_demo_Textures[iCnt].id);
    }
    glBindVertexArray(m_VAO);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<size_t>(m_indices.size()), GL_UNSIGNED_INT, 0, amount);
    glBindVertexArray(0);

    // 避免一些意料之外的bug?
    glActiveTexture(GL_TEXTURE0);
}


void demo_Mesh::SetupMesh()
{
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(demo_Vertex), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)offsetof(demo_Vertex, Normal));
    // demo_Texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)offsetof(demo_Vertex, TexCoords));
    // demo_Vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)offsetof(demo_Vertex, Tangent));
    // demo_Vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)offsetof(demo_Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)offsetof(demo_Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(demo_Vertex), (void*)offsetof(demo_Vertex, m_Weights));

    glBindVertexArray(0);
}