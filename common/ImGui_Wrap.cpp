
#include "Devices.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGuiWrap.h"
#include <cassert>
#include <filesystem>


#define EnableOutPut 0

#if EnableOutPut
#include <filesystem>
#include <iostream>
#endif

void ImGuiWrapper::ImGuiInit(GLFWwindow* win)
{
    assert(win);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();//创建上下文
    ImGuiIO& io = ImGui::GetIO();
#if EnableOutPut
    std::cout << std::filesystem::absolute(simsunPath).string().c_str() << std::endl;
#endif
    assert(std::filesystem::exists(simsunPath));
    io.Fonts->AddFontFromFileTTF(simsunPath, 24.0f, nullptr,// 设置字体
                                 io.Fonts->GetGlyphRangesChineseFull());
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;// 允许键盘控制
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // 允许游戏手柄控制
    // 设置渲染器后端
    ImGui_ImplGlfw_InitForOpenGL(win, true);
    ImGui_ImplOpenGL3_Init();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(220, 110));
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(200, 100), // 最小尺寸
        ImVec2(6000, 3000)// 最大尺寸
    );
}


void ImGuiWrapper::ImGuiNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}


void ImGuiWrapper::ImGuiDraw()
{
    ImGui::Text("Avg FPS: %.3f", ImGui::GetIO().Framerate);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void ImGuiWrapper::ImGuiCreateBasicWindow()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiWrapper::ImGuiDestroy()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}


void ImGuiWrapper::ImGuiShowCameraData(Devices& devices)
{
    if (ImGui::CollapsingHeader("Camera Paramater", true))
    {
        ImGui::SliderFloat("Cam.X", &devices.Position.x, -1000, 1000);
        ImGui::SliderFloat("Cam.Y", &devices.Position.y, -1000, 1000);
        ImGui::SliderFloat("Cam.Z", &devices.Position.z, -1000, 1000);

        ImGui::SliderFloat("Cam.upX", &devices.Up.x, -1000, 1000);
        ImGui::SliderFloat("Cam.upY", &devices.Up.y, -1000, 1000);
        ImGui::SliderFloat("Cam.upZ", &devices.Up.z, -1000, 1000);

        ImGui::SliderFloat("Cam.yaw", &devices.Yaw, -1000, 1000);
        ImGui::SliderFloat("Cam.pitch", &devices.Pitch, -1000, 1000);
    }
}

void ImGuiWrapper::ImGuiFBDebug(const std::string& texName, GLuint TexID)
{
    ImGui::Begin("FrameBuffer Debug");
    ImGui::Text("%s", texName.c_str());
    ImGui::Image(
        TexID,
        ImVec2((int)(BaseTypeLen::halfW / 5), (int)(BaseTypeLen::halfH / 5)),// 显示尺寸
        ImVec2(0, 1), ImVec2(1, 0));                                         //UV翻转
    ImGui::End();
};
