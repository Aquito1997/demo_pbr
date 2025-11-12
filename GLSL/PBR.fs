#version 330 core
layout(location = 0) out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
uniform float far_plane;
uniform bool UseShadow=false;
uniform samplerCube depthMap;

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform bool revertRough = false;

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 viewPos;

vec3 sampleOffsetDirections[20] = vec3[](
    vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
    vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
    vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
    vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));


vec3 poissonSphere[64] = vec3[](
    vec3(0.04977, -0.04471, 0.04996),
    vec3(0.01457, 0.13653, -0.06883),
    vec3(-0.16402, -0.06002, 0.13753),
    vec3(0.11800, -0.10479, -0.20073),
    vec3(-0.21864, 0.18861, -0.06015),
    vec3(0.07252, 0.33161, 0.16389),
    vec3(-0.31696, -0.19455, -0.06362),
    vec3(0.39836, 0.07348, -0.15960),
    vec3(-0.09442, -0.38786, 0.22972),
    vec3(0.17125, 0.24531, -0.31513),
    vec3(-0.10791, 0.31111, 0.36239),
    vec3(0.33045, -0.26422, -0.08869),
    vec3(-0.46476, 0.15600, 0.25888),
    vec3(0.21296, -0.35090, 0.30858),
    vec3(-0.19742, -0.42577, -0.14630),
    vec3(0.19154, 0.05417, 0.47062),
    vec3(-0.03081, 0.54966, -0.07532),
    vec3(-0.34880, 0.47016, -0.07373),
    vec3(0.49902, -0.06085, -0.25027),
    vec3(-0.21195, -0.20081, 0.48956),
    vec3(0.28719, -0.49271, -0.04665),
    vec3(-0.48943, -0.32890, 0.17835),
    vec3(0.44908, 0.38258, 0.11007),
    vec3(-0.35688, 0.32831, 0.37742),
    vec3(0.06813, -0.64678, 0.27508),
    vec3(-0.11445, 0.66219, 0.27274),
    vec3(0.60449, 0.13634, 0.16838),
    vec3(-0.46990, 0.01832, -0.46657),
    vec3(0.31620, 0.54850, -0.28475),
    vec3(-0.57644, -0.26924, -0.22955),
    vec3(0.29394, -0.11722, -0.62261),
    vec3(-0.13963, -0.61428, -0.45752),
    vec3(0.12609, 0.78173, 0.05634),
    vec3(-0.71528, 0.25872, 0.15092),
    vec3(0.45411, -0.45059, 0.30957),
    vec3(-0.16199, 0.74837, -0.33921),
    vec3(0.62796, -0.32989, 0.05800),
    vec3(-0.42048, -0.55680, -0.21214),
    vec3(0.57581, 0.23915, -0.36116),
    vec3(-0.79637, -0.09324, -0.12572),
    vec3(0.01630, -0.87133, 0.11384),
    vec3(-0.25803, -0.74941, 0.35156),
    vec3(0.71287, -0.10807, -0.40105),
    vec3(-0.39961, 0.66208, -0.18198),
    vec3(0.65294, 0.45335, -0.07025),
    vec3(-0.61016, 0.50842, 0.17899),
    vec3(0.76477, -0.44478, -0.02524),
    vec3(-0.32248, -0.85455, -0.05686),
    vec3(0.45806, 0.68357, 0.30771),
    vec3(-0.06931, 0.89850, 0.18738),
    vec3(0.88814, 0.19951, 0.17614),
    vec3(-0.26354, 0.36864, -0.74261),
    vec3(0.43736, -0.76474, -0.10845),
    vec3(-0.85387, 0.29675, -0.09258),
    vec3(0.32029, -0.58794, -0.52148),
    vec3(-0.52867, -0.52978, 0.41348),
    vec3(0.86734, -0.15458, 0.22196),
    vec3(-0.36351, -0.13132, -0.80376),
    vec3(0.62730, -0.52089, 0.37157),
    vec3(-0.88250, -0.34006, 0.13178),
    vec3(0.41426, 0.86584, -0.10748),
    vec3(-0.72164, 0.62236, -0.01363),
    vec3(0.52063, -0.70506, 0.33009),
    vec3(-0.50000, -0.50000, -0.50000)  // 补充一个对称点
);



// 法线分布函数  从统计学上近似地表示了与某些（半程）向量h取向一致的微平面的比率
float DistributionGGX(vec3 N, vec3 H, float roughness);
//几何函数 统计学上近似的求得了微平面间相互遮蔽的比率
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
// 菲涅尔方程  方程描述的是被反射的光线对比光线被折射的部分所占的比率
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
// 用于将切线空间法线转换到世界空间
vec3 GetNormalFromMap();
// 阴影计算
float ShadowCalculation(vec3 fragPos,vec3 lightPos);


const float PI = 3.14159265359;
void main() {
    vec3 mapAlbedo = pow(texture(albedoMap,TexCoords).rgb,vec3(2.2));
    float mapMetal = texture(metallicMap,TexCoords).r;
    float mapRough = texture(roughnessMap,TexCoords).r;
    float mapAO = texture(aoMap,TexCoords).r;
    if(revertRough)
        mapRough = 1 - mapRough;

    vec3 normal = GetNormalFromMap();
    vec3 viewVec = normalize(viewPos - WorldPos);
    vec3 refVec = reflect(-viewVec, normal);

    vec3 F0 = vec3(0.04);       // 入射光线的角度为90°时（垂直于法线）的一个基础反射率;
    // 预计算出平面对于法向入射的结果（F0，处于0度角，好像直接看向表面一样），
    // 然后基于相应观察角的Fresnel-Schlick近似对这个值进行插值，用这种方法来进行进一步的估算。
    F0 = mix(F0, mapAlbedo, mapMetal);

    vec3 Lo = vec3(0);// result
    for(int iCnt = 0; iCnt < 1; iCnt++) {
        vec3 lightVec = normalize(lightPositions[iCnt] - WorldPos);
        vec3 halfVec = normalize(lightVec + viewVec);
        float dist = length(lightPositions[iCnt] - WorldPos);
        float attenuation = 1 / (dist * dist);
        vec3 radiance = lightColors[iCnt] * attenuation;

        // Cook-Torrance BRDF
        float func_D = DistributionGGX(normal, halfVec, mapRough);
        float func_G = GeometrySmith(normal, viewVec, lightVec, mapRough);
        vec3 func_F = fresnelSchlick(max(dot(halfVec, viewVec), 0.0), F0);
        // vec3 func_F = fresnelSchlick(clamp(dot(halfVec, viewVec), 0.0, 1.0), F0);

        float NdotV = max(dot(normal, viewVec), 0.0);
        float NdotL = max(dot(normal, lightVec), 0.0);
        vec3 numerator = func_D * func_G * func_F;
        float denominator = 4 * NdotV * NdotL + 0.000001;
        vec3 specular = numerator / denominator;

        vec3 kS = func_F;      // kS  S: specular
        vec3 kD = vec3(1) - kS;// kD  D: diffuse
        // 将 kD 乘以 (1 - metalness)，这样只有非金属材质才有漫反射光照，
        // 如果是部分金属材质则进行线性混合（纯金属没有漫反射光照）。
        kD *= 1.0 - mapMetal;

        if(UseShadow)
        {   
            float shadow = ShadowCalculation(WorldPos, lightPositions[0]);            
            Lo += (2 * kD * mapAlbedo / PI +  specular) * radiance * NdotL * (1 - shadow);
        }
        else
            Lo += (2 * kD * mapAlbedo / PI +  specular) * radiance * NdotL;


        // 统一金属和非金属的表面基础颜色
        // Lo += (kD * mapAlbedo / PI + kS * specular) * radiance * NdotL;
        // Lo += (kD * mapAlbedo / PI + specular) * radiance * NdotL ;
    }

    vec3 F = fresnelSchlickRoughness(max(dot(normal, viewVec), 0.0), F0, mapRough);
    vec3 ks = F;
    vec3 kd = 1 - ks;
    kd *= 1 - mapMetal;

    vec3 irradiance = texture(irradianceMap, normal).rgb;
    vec3 diffuse = irradiance * mapAlbedo;

    const float MAX_REFLECTION_LOD = 2.0;
    vec3 prefilteredColor = textureLod(prefilterMap, refVec, mapRough * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewVec), 0.0), mapRough)).rg;
    
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);   
    vec3 ambient = (kd * diffuse + ks * specular) * mapAO;
    vec3 color = ambient + Lo;
    
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));
    FragColor = vec4(color, 1.0);
}




////////////////////////////////////////////////////////////////////////////////

//用于将切线空间法线转换到世界空间
vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}




float DistributionGGX(vec3 normal, vec3 halfVec, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(normal, halfVec), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    // float k = r * r / 8;// use direct light
    float k = roughness * roughness * roughness * roughness / 2;    // use IBL light

    float nom = NdotV;
    float denom = NdotV * (1 - k) + k;
    return nom / denom;
}

float GeometrySmith(vec3 normal, vec3 viewVec, vec3 lightVec, float roughness) {
    float NdotV = max(dot(normal, viewVec), 0.0);
    float NdotL = max(dot(normal, lightVec), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


float ShadowCalculation(vec3 fragPos,vec3 lightPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.05;
    int samples = 64;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = 0.05 * sqrt(viewDistance / far_plane);
    for(int iCnt = 0; iCnt < 64; ++iCnt) {
        float closestDepth = texture(depthMap, fragToLight + poissonSphere[iCnt] * diskRadius).r * far_plane;
        if(currentDepth - bias > closestDepth)  // 光源看见的值比 光源到片段的值更小，在阴影中
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}