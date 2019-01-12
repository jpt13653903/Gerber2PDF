#include "imgui.h"


namespace ImGuiExt {
    inline void MoveCursor(float delta_x, float delta_y) {
        ImGui::SetCursorPosX(ImGui::GetCursorPosX()+delta_x);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY()+delta_y);
    }

}