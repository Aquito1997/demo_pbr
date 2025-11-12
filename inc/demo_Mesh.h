#ifndef __demo_Mesh_H__
#define __demo_Mesh_H__


#include "glm/ext/vector_float3.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define MAX_BONE_INFLUENCE 4


typedef struct demo_GLSL_Material {
    size_t ambient;
    size_t diffuse;
    size_t specular;

    int useTex;// -1 轮胎材质特色处理，0 不适用纹理，1使用纹理
    float ka;  // ambient       // map_Bump
    float kd;  // diffuse       // map_kd
    float ks;  // specular      // map_Ns
} demo_GLSL_Material;


// TODO: add epsilon?
struct demo_Color3D {
    demo_Color3D() noexcept
        : r(0.0f), g(0.0f), b(0.0f)
    {
    }
    demo_Color3D(float _r, float _g, float _b)
        : r(_r), g(_g), b(_b)
    {
    }
    explicit demo_Color3D(float _r)
        : r(_r), g(_r), b(_r)
    {
    }
    demo_Color3D(const demo_Color3D& o)
        : r(o.r), g(o.g), b(o.b)
    {
    }

    std::vector<float> GetVal()
    {
        return {r, g, b};
    }

    inline demo_Color3D& operator=(const demo_Color3D& o)
    {
        r = o.r;
        g = o.g;
        b = o.b;
        return *this;
    }

    bool operator==(const demo_Color3D& other) const
    {
        return r == other.r && g == other.g && b == other.b;
    }

    bool operator!=(const demo_Color3D& other) const
    {
        return r != other.r || g != other.g || b != other.b;
    }

    bool operator<(const demo_Color3D& other) const
    {
        return r < other.r || (r == other.r && (g < other.g || (g == other.g && b < other.b)));
    }

    demo_Color3D operator+(const demo_Color3D& c) const
    {
        return demo_Color3D(r + c.r, g + c.g, b + c.b);
    }

    demo_Color3D operator-(const demo_Color3D& c) const
    {
        return demo_Color3D(r - c.r, g - c.g, b - c.b);
    }

    demo_Color3D operator*(const demo_Color3D& c) const
    {
        return demo_Color3D(r * c.r, g * c.g, b * c.b);
    }

    demo_Color3D operator*(float f) const
    {
        return demo_Color3D(r * f, g * f, b * f);
    }

    float operator[](unsigned int i) const
    {
        return *(&r + i);
    }

    float& operator[](unsigned int i)
    {
        if (0 == i)
            return r;
        else if (1 == i)
            return g;
        else if (2 == i)
            return b;
        return r;
    }

    bool IsBlack() const
    {
        static const float epsilon = float(10e-3);
        return std::fabs(r) < epsilon && std::fabs(g) < epsilon && std::fabs(b) < epsilon;
    }
    glm::vec3 data()
    {
        return glm::vec3(r, g, b);
    }
    std::string c_str()
    {
        return std::to_string(r) + std::string(",") + std::to_string(g) + std::string(",") + std::to_string(b);
    }
    float r, g, b;
};// !struct demo_Color3D

struct demo_Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int* m_BoneIDs[MAX_BONE_INFLUENCE];
    float* m_Weights[MAX_BONE_INFLUENCE];
};


struct demo_Texture {
    unsigned int id;
    std::string type;
    std::string path;
    demo_Texture() = delete;
    demo_Texture(unsigned int id_, std::string type_, std::string path_)
    {
        this->id = id_;
        this->type = type_;
        this->path = path_;
    }
    bool operator==(const demo_Texture& ver)
    {
        return id == ver.id && type == ver.type && path == ver.path;
    }
};

typedef std::pair<std::string, demo_Color3D> demo_materialColor;
typedef std::pair<std::string, float> demo_materialAttri;
typedef struct demo_Material {
    bool m_isColor;
    demo_materialColor m_color;
    demo_materialAttri m_attri;
    demo_Material() = delete;
    demo_Material(bool isColor, demo_materialColor color)
        : m_isColor(isColor), m_color(color) {};
    demo_Material(bool isColor, demo_materialAttri attri)
        : m_isColor(isColor), m_attri(attri) {};
} demo_Material;


class demo_Mesh
{
public:
    unsigned int m_VAO;
    std::vector<demo_Vertex> m_vertices;
    std::vector<demo_Texture> m_demo_Textures;
    std::vector<unsigned int> m_indices;
    std::vector<demo_Material> m_materials;

    demo_Mesh() = delete;
    demo_Mesh(std::vector<demo_Vertex> vertices, std::vector<unsigned int> indices,
              std::vector<demo_Texture> demo_Textures, bool pbrTex);
    demo_Mesh(std::vector<demo_Vertex> vertices, std::vector<unsigned int> indices,
              std::vector<demo_Texture> demo_Textures, std::vector<demo_Material> materials, bool pbrTex);


    void SetStarIndex(size_t index)
    {
        mStartIndex = index;
    };


    void SetTexture(const size_t& spID);
    void SetPbrTexture(const size_t& spID);
    void SetMaterial(const size_t& spID);
    void Draw(const size_t& spID);
    void DrawArrays(const size_t& spID, const size_t& amount);

    void ReBindTexture();

    /**
     * @brief 实例化时,绘制多个模型
     * 
     * @param spID shader program ID
     * @param amount 实例化时绘制模型的个数
     * @param index GL_demo_Texture0 + index 作为绑定纹理的起始位置.
     */
    void DrawArrays(const size_t& spID, const size_t& amount, size_t index);

private:
    void SetupMesh();

private:
    bool mPbrTex;
    size_t mStartIndex;
    bool printed = false;
    unsigned int m_VBO, m_EBO;
};

#endif