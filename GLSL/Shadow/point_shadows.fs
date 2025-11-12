#version 330 core
out vec4 FragColor;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
}
fs_in;

uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float far_plane;
uniform bool shadows;


// vec3 sampleOffsetDirections[20] = vec3[](
//     vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
//     vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
//     vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
//     vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
//     vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1));

// float ShadowCalculation(vec3 fragPos)
// {
//     // 光源到当前fragment的距离
//     vec3 fragToLight = fragPos - lightPos;
//     float currentDepth = length(fragToLight);
//     float shadow = 0.0;
//     float bias = 0.005;
//     int samples = 20;
//     float viewDistance = length(viewPos - fragPos);
//     float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
//     for(int i = 0; i < samples; ++i)
//     {
//         // 软阴影？     closestDepth：在depthmap中，光源视角采样的值
//         float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
//         // float closestDepth = texture(depthMap, fragToLight + diskRadius).r;
//         closestDepth *= far_plane;   // undo mapping [0;1]
//         if(currentDepth - bias > closestDepth)  // 光源看见的值比 光源到片段的值更小，在阴影中
//             shadow += 1.0;
//     }
//     shadow /= float(samples);
//     return shadow;
// }



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
float ShadowCalculation(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.25;
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


void main()
{
    vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    float shadow = shadows ? ShadowCalculation(fs_in.FragPos) : 0.0;
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color * 2;
    lighting *=4;
    lighting=clamp(lighting,0,1);
    FragColor = vec4(lighting, 1.0);
}
