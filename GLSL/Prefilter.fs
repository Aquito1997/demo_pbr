#version 330 core 
layout(location = 0) out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube EnvMap;
uniform float roughness;
uniform uint SAMPLE_COUNT = 1024u;
uniform float resolution = 512.0;

const float PI = 3.14159265359;
float DistributeionGGX(vec3 normal, vec3 halfVec, float roughness);
float RadicalInvecseVdc(uint bits);
vec2 Hammersley(uint i, uint n);
vec3 ImportanceSampleGGX(vec2 xi, vec3 normal, float roughenss);

void main() {
    vec3 normal = normalize(WorldPos);
    vec3 RefVec = normal;
    vec3 viewVec = RefVec;

    
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;
    for(uint iCnt = 0u; iCnt < SAMPLE_COUNT; iCnt++) {
        vec2 xi = Hammersley(iCnt, SAMPLE_COUNT);
        vec3 halfVec = ImportanceSampleGGX(xi, normal, roughness);
        vec3 lightVec = normalize(2.0 * dot(viewVec, halfVec) * halfVec - viewVec);

        float NdotL = max(dot(normal, lightVec), 0.0);
        if(NdotL > 0.0) {
            float NdotH = max(dot(normal, halfVec), 0.0);
            float HdotV = max(dot(halfVec, viewVec), 0.0);

            float ndf = DistributeionGGX(normal, halfVec, roughness);
            float pdf = ndf * NdotH / (4.0 * HdotV) + 0.00001;

            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.00001);

            float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefilteredColor += textureLod(EnvMap, lightVec, mipLevel).rgb * NdotL;
            totalWeight += NdotL;
        }
    }
    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);
}

//////////////////////////////////////////////////////////////////////////////////////

float DistributeionGGX(vec3 normal, vec3 halfVec, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, halfVec), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float RadicalInverseVdc(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N) {
    float val = float(i) / float(N);
    return vec2(val, RadicalInverseVdc(i));
}

vec3 ImportanceSampleGGX(vec2 xi, vec3 normal, float roughness) {
	float a = roughness*roughness;
	
	float phi = 2.0 * PI * xi.x;
	float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a*a - 1.0) * xi.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// from tangent-space H vector to world-space sample vector
	vec3 up          = abs(normal.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, normal));
	vec3 bitangent = cross(normal, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + normal * H.z;
	return normalize(sampleVec);
}