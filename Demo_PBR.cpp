#include "demo_PBR.h"
#include "Devices.h"
#include "Function.h"
#include "GLDefine.h"
#include "ImGuiWrap.h"
#include "VertexManager.h"
#include "glad/glad.h"
#include "glm/ext/vector_float3.hpp"
#include "inc/demo_Model.h"
#include "vertex.h"


#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"

#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <ImGui/imgui.h>

#include <cstddef>
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>

using namespace BaseTypeLen;
using namespace Function::OpenGLBase;

// sw和sh是为生产深度贴图使用的，如果是cubemap贴图，必须是sw和sh值一致
const GLuint sw = 4096, sh = 4096;
const GLfloat sa = (float)4096 / (float)4096;
// near_plane 为什么要改为 0.01（原本是1）：
// 光源是在(0,0,0)处，但是在near内（lightPos 到
// near_Plane）有正方形的部分模型，会导致正方形的阴影生成不正常（空心阴影）
static GLfloat near_plane = 0.01f, far_plane = 250.0f;// 控制深度比例
static glm::mat4 shadowProj =
    glm::perspective(glm::radians(90.0f), sa, near_plane, far_plane);

// offset 指定VAO
void Draw(RenderPipeline& render, GLuint amount,
          const std::vector<glm::mat4>& models, size_t offset)
{
    glUseProgram(render.sp);
    glBindVertexArray(render.VAO(offset));
    {
        for (auto& model : models)
        {
            glm::mat4 tmpModel = glm::scale(model, glm::vec3(0.5));
            Function::OpenGLMath::SetMat4(render.sp, "model", tmpModel);
            glDrawArrays(GL_TRIANGLES, 0, amount);
        }
    }
    glBindVertexArray(0);
}

void SetDepthMapUniformVal(const RenderPipeline& render, glm::vec3 lp)
{
    render.use();
    std::vector<glm::mat4> shadowTrans;
    shadowProj = glm::perspective(glm::radians(90.0f), (float)sw / (float)sh,
                                  near_plane, far_plane);
    shadowTrans.push_back(shadowProj * glm::lookAt(lp, lp + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTrans.push_back(shadowProj * glm::lookAt(lp, lp + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTrans.push_back(shadowProj * glm::lookAt(lp, lp + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    shadowTrans.push_back(shadowProj * glm::lookAt(lp, lp + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
    shadowTrans.push_back(shadowProj * glm::lookAt(lp, lp + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    shadowTrans.push_back(shadowProj * glm::lookAt(lp, lp + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    for (size_t xCnt = 0; xCnt < 6; xCnt++)
        Function::OpenGLMath::SetMat4(
            render.sp, "shadowMatrices[" + std::to_string(xCnt) + "]",
            shadowTrans[xCnt]);
    Function::OpenGLMath::SetFloat(render.sp, "far_plane", far_plane);
    Function::OpenGLMath::SetVec3(render.sp, "lightPos", IBL::lightPositions[0]);
}

void DrawScene(RenderPipeline& scene, GLuint offset)
{
    glUseProgram(scene.sp);
    for (size_t xCnt = 0; xCnt < scene.GetTextureCount(); xCnt++)
    {
        glActiveTexture(GL_TEXTURE0 + xCnt);
        glBindTexture(GL_TEXTURE_2D, scene.Texture(xCnt));
    }
    glBindVertexArray(scene.VAO(offset));
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void ShadowFrameBuffer(fbAttri& fbo)
{
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, sw,
                     sh, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // attach depth texture as FBO's depth buffer
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    fbo.fbo = depthMapFBO;
    fbo.PushTex(depthCubemap);
}

void ResetViewport(GLFWwindow* win)
{
    int srcW, srcH;
    glfwGetFramebufferSize(win, &srcW, &srcH);
    glViewport(0, 0, srcW, srcH);
}

void PBRShaderAttri(const std::string& basePath, RenderPipeline& pbrShader,
                    fbAttri& fb)
{
    bool flipY = true;
    pbrShader.use();
    pbrShader.BindTexture(fb.GetTexId(2), LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    pbrShader.BindTexture(fb.GetTexId(3), LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    pbrShader.BindTexture(fb.GetTexId(4), LearnGL_tt::TT_TEXTURE_2D);
    pbrShader.BindTexture(basePath + "albedo.png", LearnGL_tt::TT_TEXTURE_2D,
                          flipY);
    pbrShader.BindTexture(basePath + "ao.png", LearnGL_tt::TT_TEXTURE_2D, flipY);
    pbrShader.BindTexture(basePath + "metallic.png", LearnGL_tt::TT_TEXTURE_2D,
                          flipY);
    pbrShader.BindTexture(basePath + "normal.png", LearnGL_tt::TT_TEXTURE_2D,
                          flipY);
    pbrShader.BindTexture(basePath + "roughness.png", LearnGL_tt::TT_TEXTURE_2D,
                          flipY);
    pbrShader.SetInt("irradianceMap", 0);
    pbrShader.SetInt("prefilterMap", 1);
    pbrShader.SetInt("brdfLUT", 2);
    pbrShader.SetInt("albedoMap", 3);
    pbrShader.SetInt("aoMap", 4);
    pbrShader.SetInt("metallicMap", 5);
    pbrShader.SetInt("normalMap", 6);
    pbrShader.SetInt("roughnessMap", 7);
}

int ModulePBRShaderAttri(RenderPipeline& pbrShader, fbAttri& fb)
{
    pbrShader.use();
    pbrShader.BindTexture(fb.GetTexId(2), LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    pbrShader.BindTexture(fb.GetTexId(3), LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    pbrShader.BindTexture(fb.GetTexId(4), LearnGL_tt::TT_TEXTURE_2D);
    pbrShader.SetInt("irradianceMap", 0);
    pbrShader.SetInt("prefilterMap", 1);
    pbrShader.SetInt("brdfLUT", 2);
    return 3;
}

void BGShaderAttri(RenderPipeline& bgShader, fbAttri& fb, const Devices& dev)
{
    const glm::mat4 projection =
        glm::perspective(glm::radians(dev.Zoom), (float)halfW / (float)halfH,
                         near_plane, far_plane);

    bgShader.BindVertex(_cubeVertices, LearnGL_mt::MT_CUBE);
    bgShader.VertexAttrib(0, LearnGL_mt::MT_CUBE);
    bgShader.BindTexture(fb.GetTexId(1), LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    bgShader.SetInt("bgEnvMap", 0);
    bgShader.SetMat4("projection", projection);
}

void DrawBackground(RenderPipeline& bgShader, Devices& dev)
{
    glDisable(GL_CULL_FACE);
    const glm::mat4 projection =
        glm::perspective(glm::radians(dev.Zoom), (float)halfW / (float)halfH,
                         near_plane, far_plane);
    bgShader.use();
    bgShader.ActiveVertex();
    bgShader.ActiveTexture();
    bgShader.SetMat4("view", dev.GetViewMatrix());
    bgShader.SetMat4("projection", projection);
    bgShader.Draw();
}

void DrawLights(RenderPipeline& pbrShader, Devices& device)
{
    static GLuint vao = 0, vbo = 0, ebo = 0, indexCnt = 0;
    static glm::mat4 model;
    model = glm::translate(IMat4, IBL::lightPositions[0]);
    model = glm::scale(model, glm::vec3(0.5));

    pbrShader.use();
    pbrShader.ActiveTexture();
    pbrShader.SetMat4("view", device.GetViewMatrix());
    pbrShader.SetVec3("viewPos", device.Position);
    pbrShader.SetMat4("projection", perspective(device, halfAspect));
    pbrShader.SetVec3("lightPositions[" + std::to_string(0) + "]",
                      IBL::lightPositions[0]);
    pbrShader.SetVec3("lightColors[" + std::to_string(0) + "]",
                      IBL::lightColors[0]);
    pbrShader.SetMat4("model", model);
    pbrShader.SetMat3("NorMat", glm::transpose(glm::inverse(model)));
    IBL::DrawSphere(vao, indexCnt, vbo, ebo);
}

void DrawType59(RenderPipeline& type59Shader, Devices& device,
                demoModel& type59)
{
    glEnable(GL_CULL_FACE);
    {
        static glm::mat4 model = IMat4;
        static glm::vec3 scale = glm::vec3(1);
        static glm::vec3 trans = glm::vec3(0, -0.066f, 0);
        static glm::vec3 rotate = glm::vec3(0, 0, 0);
        IBL::ImGuiFuncWarpper::ImGuiScale(scale, "t59S", true, 0, 30);
        IBL::ImGuiFuncWarpper::ImGuiTranslate(trans, "t59T");
        IBL::ImGuiFuncWarpper::ImGuiScale(rotate, "t59R", false, -180, 180);
        model = glm::translate(IMat4, trans);
        model = glm::scale(model, scale);
        if (rotate.x != 0)
            model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1, 0, 0));
        if (rotate.y != 0)
            model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0, 1, 0));
        if (rotate.z != 0)
            model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0, 0, 1));

        type59Shader.use();
        type59Shader.ActiveTexture();
        type59Shader.SetMat4("model", model);
        type59Shader.SetVec3("viewPos", device.Position);
        type59Shader.SetMat4("view", device.GetViewMatrix());
        type59Shader.SetMat4("projection", perspective(device, halfAspect));

        type59Shader.SetInt("UseShadow", 1);
        type59Shader.SetInt("revertRough", 1);

        type59Shader.SetFloat("far_plane", far_plane);
        type59Shader.SetMat3("NorMat", glm::transpose(glm::inverse(model)));
        type59Shader.SetVec3("lightColors[" + std::to_string(0) + "]",
                             IBL::lightColors[0]);
        type59Shader.SetVec3("lightPositions[" + std::to_string(0) + "]",
                             IBL::lightPositions[0]);
        type59.Draw(type59Shader.sp);
    }
    glDisable(GL_CULL_FACE);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const std::string snowRoadPath = "./res/module/snow-road-raw-scan-freebie/Road.obj";
static const std::string snowRoadTexture_1_PATH = "./res/module/snow-road-raw-scan-freebie/Road_u1_v1.jpg";
static const std::string bootPath = "./res/module/sh_catWorkBoot_cc/";
static const std::string type59Path = "./res/module/Type59/";
static const std::string pistolPath = "./res/module/steampunk-triple-pistol-pbr/";
// 2_irradianceMap 3_prefilterMap 4_lutTex
static const std::string goldPath = "./res/PBR/gold/";
static const std::string grassPath = "./res/PBR/grass/";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    Devices device(glm::vec3(0, 3, 0));
    GLFWwindow* win = nullptr;
    glfwWindowHint(GLFW_SAMPLES, 4);
    Function::OpenGLBase::OpenGLInit(&win, halfW, halfH, "IBL PBR", ScreenRatio::SR_4K);
    EnableMouse(win, device);
    ImGuiWrapper::ImGuiInit(win);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);//立方体贴图的面之间进行正确过滤

    fbAttri framebuff;
    ShadowFrameBuffer(framebuff);

    RenderPipeline cubeMap(_cubeVertices);
    cubeMap.VertexAttrib(0, LearnGL_mt::MT_CUBE);
    cubeMap.LinkShaderProgram("./GLSL/Shadow/model.vs", "./GLSL/Shadow/model.gs",
                              "./GLSL/Shadow/model.fs");

    demoModel snowRoad(snowRoadPath, false, true);
    RenderPipeline shadowScene("./GLSL/Shadow/point_shadows.vs",
                               "./GLSL/Shadow/point_shadows.fs");

    shadowScene.BindTexture(framebuff.GetTexId(0),
                            LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    shadowScene.BindTexture(snowRoadTexture_1_PATH, LearnGL_tt::TT_TEXTURE_2D);
    shadowScene.SetInt("depthMap", 0);
    shadowScene.SetInt("diffuseTexture", 1);

    fbAttri fb;
    IBL::GenMap::GenCubeMap(fb);
    IBL::GenMap::GenEnvMap(fb);
    IBL::GenMap::GenIrradianceMap(fb);
    IBL::GenMap::GenPrefilterMap(fb);
    IBL::GenMap::GenLUTMap(fb);

    RenderPipeline bgShader("./GLSL/Background.vs", "./GLSL/Background.fs");
    BGShaderAttri(bgShader, fb, device);

    RenderPipeline pbrShader("./GLSL/PBR.vs", "./GLSL/PBR.fs");
    PBRShaderAttri(goldPath, pbrShader, fb);

    demoModel type59(type59Path + "type_59obj.obj", true, true);
    RenderPipeline type59Shader("./GLSL/Module.vs", "./GLSL/PBR.fs");
    const size_t index = ModulePBRShaderAttri(type59Shader, fb);
    type59Shader.BindTexture(framebuff.GetTexId(0),
                             LearnGL_tt::TT_TEXTURE_CUBE_MAP);
    type59Shader.SetInt("depthMap", index);// index = 3
    type59.SetStartIndex(index + 1);

    ResetViewport(win);
    while (!glfwWindowShouldClose(win))
    {
        device.UpdateCurrentFrame();
        device.processInput(win);
        ImGuiWrapper::ImGuiNewFrame();

        // 生成深度贴图
        glBindFramebuffer(GL_FRAMEBUFFER, framebuff.fbo);
        {
            glViewport(0, 0, sw, sh);
            glEnable(GL_CULL_FACE);

            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
            glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
            SetDepthMapUniformVal(cubeMap, IBL::lightPositions[0]);
            cubeMap.SetMat4("model", glm::translate(IMat4, glm::vec3(0, -0.066, 0)));
            type59.Draw(cubeMap.sp);
            ResetViewport(win);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.2, 0.2, 0.2, 1.0);

            DrawLights(pbrShader, device);
            DrawType59(type59Shader, device, type59);
            IBL::Shadow::DrawRoad(shadowScene, snowRoad, device);
            DrawBackground(bgShader, device);
        }
        ImGuiWrapper::ImGuiDraw();
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    ImGuiWrapper::ImGuiDestroy();
    glfwTerminate();
    return 0;
}
