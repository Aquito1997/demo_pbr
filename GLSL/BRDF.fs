#version 330 core
out vec2 FragColor;
in vec2 TexCoords;

const float PI = 3.14159265359;
uniform uint SAMPLE_COUNT = 1024u;

float RadicalInverseVdc(uint bits);
vec2 Hammersley(uint i, uint N);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 normal, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 normal, vec3 view, vec3 light, float roughness);
vec2 IntegrateBRDF(float NdotV, float roughness);

void main() {
    vec2 integratedBRDF = IntegrateBRDF(TexCoords.x, TexCoords.y);
    FragColor = integratedBRDF;
}
float RadicalInverseVdc(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(uint i, uint N) {
    return vec2(float(i) / float(N), RadicalInverseVdc(i));
}

vec3 ImportanceSampleGGX(vec2 Xi, vec3 normal, float roughness) {
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;

    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 halfVec = vec3(0.0);
    halfVec.x = cos(phi) * sinTheta;
    halfVec.y = sin(phi) * sinTheta;
    halfVec.z = cosTheta;

    vec3 up = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = normalize(cross(normal, tangent));

    vec3 sampleVec = tangent * halfVec.x + bitangent * halfVec.y + normal * halfVec.z;
    return normalize(sampleVec);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    // 改为平方也许效果更好
    float a = roughness;
    float k = (a * a) / 2.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 normal, vec3 view, vec3 light, float roughness) {
    float NdotV = max(dot(normal, view), 0.0);
    float NdotL = max(dot(normal, light), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);

    return ggx1 * ggx2;
}

vec2 IntegrateBRDF(float NdotV, float roughness) {
    vec3 view = vec3(0.0);
    view.x = sqrt(1.0 - NdotV * NdotV);
    view.y = 0.0;
    view.z = NdotV;

    float A = 0.0;
    float B = 0.0;

    vec3 normal = vec3(0.0, 0.0, 1.0);
    for(uint iCnt = 0u; iCnt < SAMPLE_COUNT; iCnt++) {
        vec2 Xi = Hammersley(iCnt, SAMPLE_COUNT);
        vec3 halfVec = ImportanceSampleGGX(Xi, normal, roughness);
        vec3 lightVec = normalize(2.0 * dot(view, halfVec) * halfVec - view);

        float NdotL = max(lightVec.z, 0.0);
        float NdotH = max(halfVec.z, 0.0);
        float VdotH = max(dot(view, halfVec), 0.0);

        if(NdotL > 0.0) {
            float G = GeometrySmith(normal, view, lightVec, roughness);
            float G_vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);

            A += (1.0 - Fc) * G_vis;
            B += Fc * G_vis;
        }
    }

    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}