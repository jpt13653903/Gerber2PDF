#ifndef __GUI_MAIN_H__
#define __GUI_MAIN_H__

#include "imgui.h"

struct ImGuiView {
    ImFont* normal_font;
    ImFont* title_font;
};

void gui_setup(int argc, char **argv);
void gui_loop();

#endif // __GUI_MAIN_H__