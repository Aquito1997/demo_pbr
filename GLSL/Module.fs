#version 330 core
out vec4 FragColor;
uniform sampler2D diffTex;
uniform sampler2D specTex;

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {
    vec3 diff = texture(diffTex,TexCoords).xyz;
    float spec = texture(specTex,TexCoords).x;
    FragColor = vec4(diff + vec3(spec) / (diff + vec3(spec) + 0.01),1);
}
