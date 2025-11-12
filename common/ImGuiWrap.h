/**
 * @file       ImGui_Wrap.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2025-08-08
 * @version    v1.0.0
 * @copyright  Copyright (c) 2025
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2025/08/08 Time: 11:16   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */

#ifndef __ImGui_Wrap_H__
#define __ImGui_Wrap_H__

#include "Devices.h"
#include <GLFW/glfw3.h>

namespace ImGuiWrapper
{
void ImGuiInit(GLFWwindow* win);
void ImGuiNewFrame();
void ImGuiDraw();
void ImGuiCreateBasicWindow();
void ImGuiDestroy();
void ImGuiShowCameraData(Devices& camera);
void ImGuiFBDebug(const std::string& texName, GLuint TexID);
}// namespace ImGuiWrapper


#endif