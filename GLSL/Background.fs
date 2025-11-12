#version 330 core
layout(location = 0) out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube bgEnvMap;
void main() {
    vec3 envColor = textureLod(bgEnvMap, WorldPos, 0.0).rgb;

    // HDR
    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0 / 2.2));

    FragColor = vec4(envColor, 1.0);
}