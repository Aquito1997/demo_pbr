#include "VertexManager.h"
#include "Function.h"
#include "GLDefine.h"
#include "StackTracer.hpp"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>


using namespace BaseTypeLen;

VertexManager::VertexManager(const float* vboData, size_t vboSize)
{
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    for (size_t iCnt = 0; iCnt < 1; iCnt++)
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vboSize, vboData, GL_STATIC_DRAW);
    }
    glBindVertexArray(0);
    PushABPair(vao, vbo);
};

void VertexManager::Init(const float* vboData, size_t vboSize,
                         LearnGL_mt type)
{
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vboSize, vboData, GL_STATIC_DRAW);
    glBindVertexArray(0);
    PushABPair(vao, vbo, type);
}

void VertexManager::Init(const std::vector<glm::mat4>& insMat,
                         LearnGL_mt type)
{
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, insMat.size() * mat4Size, insMat.data(),
                 GL_STATIC_DRAW);

    glBindVertexArray(0);
    PushABPair(vao, vbo, type);
}

void VertexManager::PushABPair(GLuint vao, GLuint vbo, LearnGL_mt type)
{
    VAO_VBO ab = {vao, vbo, type};
    vao_vbo.push_back(ab);
}

const VAO_VBO& VertexManager::GetABPair(size_t index)
{
    return vao_vbo[index];
}

void VertexManager::PushABPair(VAO_VBO abp)
{
    vao_vbo.push_back(abp);
}

void VertexManager::SetVertexAttribute_cube(int index)
{
    GLuint& vao = vao_vbo[index].VAO;
    glBindVertexArray(vao);
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * FltSize,
                              (void*)(0 * FltSize));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * FltSize,
                              (void*)(3 * FltSize));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * FltSize,
                              (void*)(6 * FltSize));
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// 切线空间
void VertexManager::SetVertexAttribute_tangent(int index)
{
    GLuint& vao = vao_vbo[index].VAO;
    glBindVertexArray(vao);
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * FltSize,
                              (void*)(0 * FltSize));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * FltSize,
                              (void*)(3 * FltSize));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * FltSize,
                              (void*)(6 * FltSize));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * FltSize,
                              (void*)(8 * FltSize));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * FltSize,
                              (void*)(11 * FltSize));
    }
    glBindVertexArray(0);
}

void VertexManager::SetVertexAttribute_quad(int index)
{
    glBindVertexArray(vao_vbo[index].VAO);
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * FltSize,
                              (void*)(0 * FltSize));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * FltSize,
                              (void*)(3 * FltSize));
    }
    glBindVertexArray(0);
}

void VertexManager::SetVertexAttribute_InstanceMatrix(int index)
{
    glBindVertexArray(vao_vbo[index].VAO);
    {
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                              (void*)(0 * vec4Size));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                              (void*)(1 * vec4Size));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                              (void*)(2 * vec4Size));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,
                              (void*)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }
    glBindVertexArray(0);
}

size_t VertexManager::VertexNumber() const
{
    return vao_vbo.size();
}

GLuint& VertexManager::VAO(size_t index)
{
    static constexpr size_t sizeT_zero = 0;
    if (vao_vbo.size() < sizeT_zero)
    {
        Function::LinuxDebug::PrintStackTrace();
        exit(0);
    }

    return vao_vbo[index].VAO;
}

GLuint& VertexManager::VBO(size_t index)
{
    return vao_vbo[index].VBO;
}

size_t VertexManager::size() const
{
    return vao_vbo.size();
}

void VertexManager::Draw()
{
    for (auto& ab : vao_vbo)
    {
        if (ab.type == LearnGL_mt::MT_CUBE)
            glDrawArrays(GL_TRIANGLES, 0, 36);
        else if (ab.type == LearnGL_mt::MT_QUAD)
            glDrawArrays(GL_TRIANGLES, 0, 6);
        else
        {
            std::cout << __FILE__ << ":" << __LINE__ << " not implement" << std::endl;
            assert(false);
        }
    }
}

VertexManager::~VertexManager()
{
    for (auto xAB : vao_vbo)
    {
        glDeleteVertexArrays(1, &xAB.VAO);
        glDeleteBuffers(1, &xAB.VBO);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

RenderPipeline::RenderPipeline()
{
    sp = 0;
}

RenderPipeline::RenderPipeline(const float* vboData, size_t vboSize)
{
    sp = 0;
    BindVertex(vboData, vboSize);
};

RenderPipeline::RenderPipeline(const std::vector<GLfloat> vecVertex)
{
    sp = 0;
    BindVertex(vecVertex);
};

RenderPipeline::RenderPipeline(const std::string& vs, const std::string& fs)
{
    LinkShaderProgram(vs, fs);
    use();
}

RenderPipeline::RenderPipeline(const std::string& vs, const std::string& gs,
                               const std::string& fs)
{
    LinkShaderProgram(vs, gs, fs);
}

RenderPipeline::~RenderPipeline()
{
    for (size_t xCnt = 0; xCnt > vecTex.size(); xCnt++)
    {
        glDeleteTextures(1, &vecTex[xCnt].texId);
    }
    glDeleteProgram(sp);
}

size_t RenderPipeline::GetTextureCount() const
{
    return vecTex.size();
}

void RenderPipeline::BindTexture(const std::string& texPath, LearnGL_tt texType,
                                 bool flipY)
{
    if (!std::filesystem::exists(texPath))
    {
        std::cout << __FILE__ << ":" << __LINE__ << "  texPath: " << texPath << "\t\n";
        assert(std::filesystem::exists(texPath));
    }
    LearnGL_ti ti;
    if (texType == LearnGL_tt::TT_TEXTURE_2D)
    {
        ti.texId = Function::OpenGLBase::BindTextureBuffer(texPath.c_str(), flipY);
    }
    else if (texType == LearnGL_tt::TT_TEXTURE_CUBE_MAP)
    {
        ti.texId =
            Function::OpenGLBase::BindTextureBuffer_Cubemap(texPath.c_str(), flipY);
    }
    ti.texType = texType;
    vecTex.push_back(ti);
}

void RenderPipeline::BindTexture(GLuint id, LearnGL_tt texType)
{
    use();
    vecTex.push_back(LearnGL_ti(id, texType));
}

// 创建一个空的texture id
void RenderPipeline::CreateTexture(LearnGL_tt texType)
{
    GLuint tex = 0;
    vecTex.push_back(LearnGL_ti(tex, texType));
}

void RenderPipeline::PutTexId(GLuint index, GLuint id, LearnGL_tt tt)
{
    if (index >= vecTex.size())
        std::cout << __FILE__ << ":" << __LINE__ << " : " << "out of range"
                  << std::endl;
    vecTex[index] = LearnGL_ti(id, tt);
}

GLuint RenderPipeline::GetTexId(GLuint index)
{
    if (index >= vecTex.size())
    {
        GL_TRACE("Out of range");
        abort();
        return 0;
    }
    return vecTex[index].texId;
}

GLuint RenderPipeline::GetTexType(GLuint index)
{
    if (index >= vecTex.size())
    {
        GL_TRACE("Out of range");
        abort();
        return 0;
    }
    return vecTex[index].texType;
}

const LearnGL_ti& RenderPipeline::GetTextureInfo(GLuint index) const
{
    if (index >= vecTex.size())
    {
        GL_TRACE("Out of range");
        abort();
    }
    return vecTex[index];
}

// vs & fs
void RenderPipeline::LinkShaderProgram(const std::string& vs,
                                       const std::string& fs)
{
    sp = Function::LoadShaderProgram(vs, fs);
}

// vs、gs、fs
void RenderPipeline::LinkShaderProgram(const std::string& vs,
                                       const std::string& gs,
                                       const std::string& fs)
{
    sp = Function::BindShaderProgram(vs, gs, fs);
}

const GLuint& RenderPipeline::VAO(size_t index)
{
    return this->vm.VAO(index);
}
const GLuint& RenderPipeline::VBO(size_t index)
{
    return this->vm.VBO(index);
}

const VAO_VBO& RenderPipeline::GetABPair(size_t index)
{
    return this->vm.GetABPair(index);
}

void RenderPipeline::ActiveVertex(size_t index)
{
    use();
    glBindVertexArray(VAO(index));
}

void RenderPipeline::PushABPair(const VAO_VBO& abp)
{
    this->vm.PushABPair(abp);
}

void RenderPipeline::PushABPair(const GLuint vao, const GLuint vbo)
{
    this->vm.PushABPair(vao, vbo);
}

void RenderPipeline::BindVertex(const float* vboData, size_t vboSize,
                                LearnGL_mt type)
{
    assert(vboData);
    vm.Init(vboData, vboSize, type);
    VertexAttrib(vm.size() - 1, type);
}

void RenderPipeline::BindVertex(const std::vector<GLfloat>& vecVertex,
                                LearnGL_mt type)
{
    assert(vecVertex.size() > 0);
    vm.Init(vecVertex.data(), vecVertex.size() * sizeof(vecVertex[0]), type);
}

void RenderPipeline::BindVertex(const std::vector<glm::mat4>& vecMat4,
                                LearnGL_mt type)
{
    if (vecMat4.size() == 0)
    {
        GL_TRACE("Empty container");
        return;
    }
    vm.Init(vecMat4, type);
}

void RenderPipeline::VertexAttrib(int index, LearnGL_mt type)
{
    if (type == LearnGL_mt::MT_CUBE)
    {
        vm.SetVertexAttribute_cube(index);
    }
    else if (type == LearnGL_mt::MT_TANGENT)
    {
        vm.SetVertexAttribute_tangent(index);
    }
    else if (type == LearnGL_mt::MT_QUAD)
    {
        vm.SetVertexAttribute_quad(index);
    }
    else if (type == LearnGL_mt::MT_INSTANCE_MATRIX4)
    {
        vm.SetVertexAttribute_InstanceMatrix(index);
    }
}

void RenderPipeline::PrintAllData()
{
    std::cout << "Vertex Data:" << std::endl;
    for (size_t xCnt = 0; xCnt < vm.VertexNumber(); xCnt++)
    {
        std::cout << "Group " << xCnt + 1 << ":  "
                  << "VAO: " << vm.VAO(xCnt) << "    "
                  << "VBO: " << vm.VBO(xCnt) << std::endl;
    }
    std::cout << "\n";

    if (vecTex.size() > 0)
    {

        std::cout << "Texture Data:" << std::endl;
        for (size_t xCnt = 0; xCnt < vecTex.size(); xCnt++)
        {
            std::cout << "Group " << xCnt + 1 << ":  "
                      << "TexId: " << vecTex[xCnt].texId << "    ";
            if (vecTex[xCnt].texType == GL_TEXTURE_2D)
                std::cout << "TexType: " << "GL_TEXTURE_2D" << std::endl;
            else if (vecTex[xCnt].texType == GL_TEXTURE_CUBE_MAP)
                std::cout << "TexType: " << "GL_TEXTURE_CUBE_MAP" << std::endl;
        }
        std::cout << "\n";
    }

    std::cout << "Shader Program Id:" << sp << std::endl;
    std::cout << "==============================================\n\n"
              << std::endl;
}

GLuint& RenderPipeline::Texture(size_t index)
{
    return this->vecTex[index].texId;
}

void RenderPipeline::ActiveTexture() const
{
    use();
    for (size_t xCnt = 0; xCnt < vecTex.size(); xCnt++)
    {
        glActiveTexture(GL_TEXTURE0 + xCnt);
        glBindTexture(vecTex[xCnt].texType, vecTex[xCnt].texId);
    }
}

void RenderPipeline::DeActiveTexture()
{
    glUseProgram(this->sp);
    for (size_t xCnt = 0; xCnt < vecTex.size(); xCnt++)
    {
        glActiveTexture(GL_TEXTURE0 + xCnt);
        glBindTexture(vecTex[xCnt].texType, 0);
    }
}

void RenderPipeline::SetTextureAttrib(const std::string uniformStr,
                                      const int& val) const
{
    use();
    this->SetInt(uniformStr, val);
}

void RenderPipeline::use() const
{
    assert(sp);
    glUseProgram(sp);
}

void RenderPipeline::Draw()
{
    vm.Draw();
}

void RenderPipeline::SetBool(const std::string uniformStr,
                             const bool& val) const
{
    return this->SetInt(uniformStr, val);
}

void RenderPipeline::SetInt(const std::string uniformStr,
                            const int& val) const
{
    return this->SetInt(uniformStr.c_str(), val);
}
void RenderPipeline::SetInt(const char* uniformStr, const int& val) const
{
    use();
    const GLuint& spu = glGetUniformLocation(this->sp, uniformStr);
    glUniform1i(spu, val);
    return;
}

void RenderPipeline::SetFloat(const std::string uniformStr,
                              const float& val) const
{
    return this->SetFloat(uniformStr.c_str(), val);
}
void RenderPipeline::SetFloat(const char* uniformStr, const float& val) const
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform1f(spu, val);
    return;
}

void RenderPipeline::SetVec2(const std::string uniformStr,
                             const glm::vec2& val) const
{
    return this->SetVec2(uniformStr.c_str(), val);
}
void RenderPipeline::SetVec2(const char* uniformStr,
                             const glm::vec2& val) const
{
    const GLuint& spu = glGetUniformLocation(sp, uniformStr);
    glUniform2fv(spu, 1, glm::value_ptr(val));
    return;
}

void RenderPipeline::SetVec3(const std::string uniformStr,
                             const glm::vec3& val) const
{
    use();
    return this->SetVec3(uniformStr.c_str(), val);
}
void RenderPipeline::SetVec3(const char* uniformStr,
                             const glm::vec3& val) const
{
    use();
    const GLuint& spu = glGetUniformLocation(this->sp, uniformStr);
    glUniform3fv(spu, 1, glm::value_ptr(val));
    return;
}

void RenderPipeline::SetVec4(const std::string uniformStr,
                             const glm::vec4& val) const
{
    return this->SetVec4(uniformStr.c_str(), val);
}
void RenderPipeline::SetVec4(const char* uniformStr,
                             const glm::vec4& val) const
{
    const GLuint& spu = glGetUniformLocation(this->sp, uniformStr);
    glUniform4fv(spu, 1, glm::value_ptr(val));
    return;
}

void RenderPipeline::SetMat3(const std::string uniformStr,
                             const glm::mat3& val) const
{
    return this->SetMat3(uniformStr.c_str(), val);
}
void RenderPipeline::SetMat3(const char* uniformStr,
                             const glm::mat3& val) const
{
    use();
    const GLuint& spu = glGetUniformLocation(this->sp, uniformStr);
    glUniformMatrix3fv(spu, 1, false, glm::value_ptr(val));
    return;
}

void RenderPipeline::SetMat4(const std::string uniformStr,
                             const glm::mat4& val) const
{
    return this->SetMat4(uniformStr.c_str(), val);
}
void RenderPipeline::SetMat4(const char* uniformStr,
                             const glm::mat4& val) const
{
    use();
    const GLuint& spu = glGetUniformLocation(this->sp, uniformStr);
    glUniformMatrix4fv(spu, 1, false, glm::value_ptr(val));
    return;
}